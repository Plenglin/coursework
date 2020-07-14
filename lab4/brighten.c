#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

typedef struct tagBITMAPFILEHEADER {
    WORD bfType; //specifies the file type
    DWORD bfSize; //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
} tagBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize; //specifies the number of bytes required by the struct
    LONG biWidth; //specifies width in pixels
    LONG biHeight; //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage; //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed; //number of colors used by th ebitmap
    DWORD biClrImportant; //number of colors that are important
} tagBITMAPINFOHEADER;

// This is the order of how colors are stored.
typedef struct Color {
    unsigned char b;
    unsigned char g; 
    unsigned char r;
} Color;

typedef struct FloatColor {
    float r;
    float g;
    float b;
} FloatColor;

FloatColor color_to_float(Color color) {
    FloatColor out;
    out.r = (float)(color.r & 0xff);
    out.g = (float)(color.g & 0xff);
    out.b = (float)(color.b & 0xff);
    return out;
}

Color floatcolor_to_color(FloatColor color) {
    Color out;
    out.r = (int)(color.r);
    out.g = (int)(color.g);
    out.b = (int)(color.b);
    return out;
}

/**
 * Class for accessing pixels from a bitmap pixel array.
 */
typedef struct Image {
    int width;
    int height;
    int row_size;
    int pixels_size;
    char *pixel_data;
} Image;

Image* create_image(int width, int height, int pixels_size) {
    Image *image = (Image*) mmap(NULL, sizeof(Image), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    char *pixel_data = (char*) mmap(NULL, pixels_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    image->width = width; 
    image->height = height; 
    image->row_size = ((24 * width + 31) / 32) * 4;  // 4 * ceil((24 * width) / 32)
    image->pixels_size = pixels_size;
    image->pixel_data = pixel_data;

    return image;
}

/**
 * Returns a pointer to the pixel at the requested position.
 */
Color* get_pixel(Image *image, int x, int y) {
    return (Color*)(image->pixel_data + y * image->row_size + 3 * x);
}

/**
 * Convenience function for reading a pixel and converting to a float.
 */
FloatColor get_pixel_float(Image *image, int x, int y) {
    Color color = *get_pixel(image, x, y);
    return color_to_float(color);
}

void dispose_image(Image *image) {
    munmap(image->pixel_data, image->pixels_size);
}

/**
 * Holds all the useful data from a bitmap together.
 */
typedef struct BitmapImageData {
    tagBITMAPFILEHEADER file_header;
    tagBITMAPINFOHEADER info_header;
    Image *image;
} BitmapImageData;

void dispose_bmp(BitmapImageData *image) {
    if (image->image != NULL) {
        dispose_image(image->image);
    }
}

int read_bitmap(BitmapImageData *image, char *path) {   
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        return 1;
    }

    // Read the file header
    fread(&image->file_header, 14, 1, file);

    // Read the info header size
    fread(&image->info_header.biSize, 4, 1, file);

    // Read the rest of the info header, sized based on the header size
    fread((char*)&image->info_header.biSize + 4, image->info_header.biSize - 4, 1, file);

    // Read the pixels
    image->image = create_image(image->info_header.biWidth, image->info_header.biHeight, image->info_header.biSizeImage);
    fread(&image->image->pixel_data, image->info_header.biSizeImage, 1, file);

    fclose(file);
    return 0;
}

int write_bitmap(BitmapImageData *data, char* path) {
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        return 1;
    }

    fwrite((char*) &data->file_header, 14, 1, file);
    fwrite((char*) &data->info_header, data->info_header.biSize, 1, file);
    fwrite((char*) &data->image->pixel_data, data->info_header.biSizeImage, 1, file);
    
    fclose(file);
    return 0;
}

void print_help_text(char *exec) {
    printf(
        "Usage: %s <input> <brightness> <parallel> <output>\n\n"
        "  input               the input images\n"
        "  brightness          a number describing how much brightness to add, should \n"
        "                      be within [0, 1]\n",
        "  parallel            1 if you want to go super fast, 0 otherwise\n",
        "  output              where to write the output\n", 
        exec
    );
}

int main(int argc, char *argv[]) {
    BitmapImageData *image = (BitmapImageData*) malloc(sizeof(BitmapImageData));
    char path_a[] = "lab4/flowers.bmp";
    char path_b[] = "lab4/out.bmp";
    read_bitmap(image, path_a);
    write_bitmap(image, path_b);
    return 0;
}
