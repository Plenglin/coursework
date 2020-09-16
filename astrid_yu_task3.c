#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

typedef struct tagBITMAPFILEHEADER {
	char bfType[2];  //specifies the file type
	int bfSize;  //specifies the size in bytes of the bitmap file
	WORD bfReserved1;  //reserved; must be 0
	WORD bfReserved2;  //reserved; must be 0
	DWORD bfOffBits;  //species the offset in bytes 
} tagBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;  //specifies the number of bytes required by the struct
	LONG biWidth;  //specifies width in pixels
	LONG biHeight;  //species height in pixels
	WORD biPlanes; //specifies the number of color planes, must be 1
	WORD biBitCount; //specifies the number of bit per pixel
	DWORD biCompression;//spcifies the type of compression
	DWORD biSizeImage;  //size of image in bytes
	LONG biXPelsPerMeter;  //number of pixels per meter in x axis
	LONG biYPelsPerMeter;  //number of pixels per meter in y axis
	DWORD biClrUsed;  //number of colors used by th ebitmap
	DWORD biClrImportant;  //number of colors that are important
} tagBITMAPINFOHEADER;

typedef struct Color {
    unsigned char b, g, r;
} Color;

typedef struct Bitmap {
    tagBITMAPINFOHEADER info;
    tagBITMAPFILEHEADER file;
    int row_size;
    int *pixels;
} Bitmap;

typedef struct FloatColor {
    float r, g, b;
} FloatColor;

float interp(float t, float a, float b) {
    return (b - a) * t + a;
}

FloatColor interp_color(float t, FloatColor a, FloatColor b) {
    FloatColor color = {
        interp(t, a.r, b.r),
        interp(t, a.g, b.g),
        interp(t, a.b, b.b)
    };
    return color;
}

Color* get_pixel(Bitmap *bmp, int x, int y) {
    return (Color*) ((char*)bmp->pixels + bmp->row_size * y + 3 * x);
}

FloatColor to_float(Color color) {
    FloatColor out = {
        color.r,
        color.g,
        color.b
    };
    return out;
}

Color to_pix(FloatColor color) {
    Color out;
    out.r = color.r;
    out.g = color.g;
    out.b = color.b;
    return out;
}

FloatColor get_bilinear(Bitmap *bmp, float x, float y) {
    int x0 = x;
    int x1 = x0 + 1;
    int y0 = y; 
    int y1 = y0 + 1;
    float dx = x - x0;
    float dy = y - y0;

    return interp_color(
        dy,
        interp_color(
            dx,
            to_float(*get_pixel(bmp, x0, y0)),
            to_float(*get_pixel(bmp, x1, y0))
        ),
        interp_color(
            dx,
            to_float(*get_pixel(bmp, x0, y1)),
            to_float(*get_pixel(bmp, x1, y1))
        )
    );
}

void read_bitmap(char *path, Bitmap *bitmap) {
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        exit(-1);
    }

    fread((char*)&bitmap->file, 1, 14, file);
    fread((char*)&bitmap->info.biSize, 1, 4, file);
    fread((char*)&bitmap->info + 4, 1, bitmap->info.biSize - 4, file);

    bitmap->pixels = malloc(bitmap->info.biSizeImage);
    fread((char*)bitmap->pixels, 1, bitmap->info.biSizeImage, file);

    bitmap->row_size = ((24 * bitmap->info.biWidth + 31) / 32) * 4;
    
    // CLOSE!!!
    fclose(file);
}

void write_bitmap(char *path, Bitmap *bitmap) {
    FILE *file = fopen(path, "w");

    fwrite((char*)&bitmap->file, 1, 14, file);
    fwrite((char*)&bitmap->info, 1, bitmap->info.biSize, file);
    fwrite((char*)bitmap->pixels, 1, bitmap->info.biSizeImage, file);

    // CLOSE!!!
    fclose(file);
}

void mix(Bitmap *dst, Bitmap *src) {
    for (int y = 0; y < dst->info.biHeight; y++) {
        for (int x = 0; x < dst->info.biWidth; x++) {
            Color *dpix = get_pixel(dst, x, y);
            FloatColor dc = to_float(*dpix);
            FloatColor sc = get_bilinear(
                src, 
                (float)x * src->info.biWidth / dst->info.biWidth, 
                (float)y * src->info.biHeight / dst->info.biHeight
            );

            *dpix = to_pix(interp_color(0.5, dc, sc));
        }
    }
}


void mix_paths(char *path_a, char *path_b, char *path_out) {
    Bitmap a, b;
    read_bitmap(path_a, &a);
    read_bitmap(path_b, &b);

    Bitmap *larger = &a;
    Bitmap *smaller = &b;
    
    mix(larger, smaller);
   
    write_bitmap(path_out, larger);

    // DISPOSE!!!
    free(a.pixels);
    free(b.pixels);
}

int main() {
    mix_paths("nopadding.bmp", "yespadding.bmp", "result.bmp");
}
