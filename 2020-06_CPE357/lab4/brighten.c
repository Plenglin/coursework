#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void initialize_image(Image *image, int width, int height, int pixels_size) {
    char *pixel_data = (char*) mmap(NULL, pixels_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    image->width = width; 
    image->height = height; 
    image->row_size = ((24 * width + 31) / 32) * 4;  // 4 * ceil((24 * width) / 32)
    image->pixels_size = pixels_size;
    image->pixel_data = pixel_data;
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

unsigned char add_brightness(unsigned char c, int amount) {
    int out = c + amount;
    if (out < 0) {
        return 0;
    }
    if (out > 255) {
        return 255;
    }
    return out;
}

#define LOWER 1
#define UPPER 2

int add_brightness(Image *image, float factor, int region_flag) {
    int amount = (int) (255 * factor);
    int lower_half = image->height / 2;
    
    int y_offset, y_end;
    switch (region_flag) {
        case LOWER:
            y_offset = 0;
            y_end = lower_half;
            break;
        case UPPER:
            y_offset = lower_half;
            y_end = image->height;
            break;
        case LOWER | UPPER:
            y_offset = 0;
            y_end = image->height;
            break;
        default:
            return 1;
    }

    for (int y = y_offset; y < y_end; y++) {
        for (int x = 0; x < image->width; x++) {
            Color *color = get_pixel(image, x, y);
            color->r = add_brightness(color->r, amount);
            color->g = add_brightness(color->g, amount);
            color->b = add_brightness(color->b, amount);
        }
    }
    return 0;
}

/**
 * Holds all the useful data from a bitmap together.
 */
typedef struct BitmapImageData {
    tagBITMAPFILEHEADER file_header;
    tagBITMAPINFOHEADER info_header;
    Image image;
} BitmapImageData;

void dispose_bmp(BitmapImageData *image) {
    dispose_image(&image->image);
}

int read_bitmap(BitmapImageData *image, char *path) {   
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        return 1;
    }

    // Read the file header
    fread(&image->file_header, 1, 14, file);

    // Read the info header size
    fread(&image->info_header.biSize, 4, 1, file);

    // Read the rest of the info header, sized based on the header size
    fread((char*)&image->info_header.biSize + 4, 1, image->info_header.biSize - 4, file);

    // Read the pixels
    initialize_image(&image->image, image->info_header.biWidth, image->info_header.biHeight, image->info_header.biSizeImage);
    fread(image->image.pixel_data, 1, image->info_header.biSizeImage, file);
    //fwrite(image->image.pixel_data,1,image->image.pixels_size,stdout);

    if (fclose(file)) {
        return 1;
    }
    return 0;
}

int write_bitmap(BitmapImageData *data, char* path) {
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        return 1;
    }

    fwrite(&data->file_header, 1, 14, file);
    fwrite(&data->info_header, 1, data->info_header.biSize, file);
    fwrite(data->image.pixel_data, 1, data->info_header.biSizeImage, file);
    
    if (fclose(file)) {
        return 1;
    }
    return 0;
}

void print_help_text(char *exec) {
    printf(
        "Usage: %s <input> <brightness> <parallel> <output>\n\n"
        "  input               the input images\n"
        "  brightness          a number describing how much brightness to add, should \n"
        "                      be within [0, 1]\n"
        "  parallel            1 if you want to go super fast, 0 otherwise\n"
        "  output              where to write the output\n",
        exec
    );
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Error: wrong number of arguments.\n");
        print_help_text(argv[0]);
    }

    char *path_a = argv[1];
    char *path_b = argv[4];
    float brightness = atof(argv[2]);
    char parallel = atoi(argv[3]);

    BitmapImageData image;
    
    if (read_bitmap(&image, path_a)) {
        printf("Error: reading input failed.\n");
        return -1;
    }

    clock_t start = clock();
    if (parallel) {
        printf("Running parallel\n");
        if (fork() == 0) {
            add_brightness(&image.image, brightness, LOWER);
            exit(0);
        } else {
            add_brightness(&image.image, brightness, UPPER);
            wait(0);
        }
    } else {
        printf("Running single-threaded\n");
        add_brightness(&image.image, 0.5, LOWER | UPPER);
    }
    clock_t end = clock();

    printf("Finished in %lfms\n", 1000 * (double)(end - start) / CLOCKS_PER_SEC);

    if (write_bitmap(&image, path_b)) {
        printf("Error: writing output failed.\n");
    }
    dispose_bmp(&image);
    return 0;
}
