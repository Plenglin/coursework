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
    int *pixels;
} Bitmap;

typedef struct FloatColor {
    float r, g, b;
} FloatColor;

float interpolate(float t, float a, float b) {
    return (b - a) * t + a;
}

void read_bitmap(char *path, Bitmap *bitmap) {
    FILE *file = fopen(path, "r");

    fread(&bitmap->file, 1, 14, file);
    fread(&bitmap->info, 1, 40, file);

    bitmap->pixels = malloc(bitmap->info.biSizeImage);
    //fseek(file, bitmap->file.bfOffBits);
    fread(bitmap->pixels, 1, bitmap->info.biSizeImage, file);

    // CLOSE!!!
    fclose(file);
}

void write_bitmap(char *path, Bitmap *bitmap) {
    FILE *file = fopen(path, "w");

    fwrite(&bitmap->file, 1, 14, file);
    fwrite(&bitmap->info, 1, 40, file);
    fwrite(bitmap->pixels, 1, bitmap->info.biSize, file);

    // CLOSE!!!
    fclose(file);
}


void mix(char *path_a, char *path_b, char *path_out) {
    Bitmap a;
    read_bitmap(path_a, &a);
    write_bitmap(path_out, &a);
    

    // DISPOSE!!!
    free(a.pixels);
}

int main() {
    mix("nopadding.bmp", "yespadding.bmp", "result.bmp");
}
