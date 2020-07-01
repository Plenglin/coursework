#include <string>
#include <iostream>
#include <fstream>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

struct tagBITMAPFILEHEADER {
    WORD bfType; //specifies the file type
    DWORD bfSize; //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct tagBITMAPINFOHEADER {
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
};

struct Color {
    char b;
    char g; 
    char r;
};

void print_color(Color color) {
    std::cout << "rgb(" << (color.r & 0xff) << "," << (color.g & 0xff) << "," << (color.b & 0xff) << ")";
}

class Image {
public:
    const int width;
    const int height;
    const int row_size;
    char * const pixel_data;

    Image(int width, int height, int pixels_size) : 
        width(width), 
        height(height), 
        row_size(((24 * width + 31) / 32) * 4),
        pixel_data(new char[pixels_size]) 
    { }

    Color get_pixel(int x, int y) {
        return *(Color*)(pixel_data + (height - y + 1) * row_size + 3 * x);
    }

    ~Image() {
        delete pixel_data;
    }
};


Image *read_bitmap(std::string path) {
    tagBITMAPFILEHEADER file_header = {0};
    tagBITMAPINFOHEADER info_header = {0};
    std::ifstream file(path, std::ifstream::binary);  // gets auto-closed when destroyed
    file.read((char*) &file_header, 14);
    file.read((char*) &info_header.biSize, 4);
    file.read(((char*) &info_header.biSize) + 4, info_header.biSize - 4);

    Image *image = new Image(info_header.biWidth, info_header.biHeight, info_header.biSizeImage);
    file.read(image->pixel_data, info_header.biSizeImage);
    return image;
}

int main() {
    Image *img = read_bitmap("prog1/flowers.bmp");
    print_color(img->get_pixel(0, 0));
}
