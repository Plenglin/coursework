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

struct FloatColor {
    float r;
    float g;
    float b;
};

FloatColor color_to_float(Color color) {
    FloatColor out;
    out.r = float(color.r & 0xff);
    out.g = float(color.g & 0xff);
    out.b = float(color.b & 0xff);
}

Color floatcolor_to_color(FloatColor color) {
    Color out;
    out.r = char(color.r);
    out.g = char(color.g);
    out.b = char(color.b);
    return out;
}

/**
 * Linear interpolation between 2 values.
 * :param t the interpolation in [0, 1]
 * :param a first value
 * :param b second value
 * :return the value between
 */
float interp(float t, float a, float b) {
    return (b - a) * t + a;
}

FloatColor interp(float t, FloatColor a, FloatColor b) {
    return {interp(t, a.r, b.r), interp(t, a.g, b.g), interp(t, a.b, b.b)};
}

void print_color(Color color) {
    std::cout << "rgb(" << (color.r & 0xff) << "," << (color.g & 0xff) << "," << (color.b & 0xff) << ")";
}

class Image {
public:
    const int width;
    const int height;
    const int row_size;
    FloatColor *pixels;
    char * const pixel_data;

    Image(int width, int height, int pixels_size) : 
        width(width), 
        height(height), 
        row_size(((24 * width + 31) / 32) * 4),
        pixel_data(new char[pixels_size]) 
    { }

    Color *get_pixel(int x, int y) {
        return (Color*)(pixel_data + (height - y + 1) * row_size + 3 * x);
    }

    FloatColor get_pixel_float(int x, int y) {
        Color color = *get_pixel(x, y);
        return color_to_float(color);
    }

    FloatColor interp_pixel(float x, float y) {
        int x0 = int(x);
        int y0 = int(y);
        int x1 = x0 + 1;
        int y1 = y0 + 1;
        float tx = x - x0;
        float ty = y - y0;
        
        FloatColor c0 = interp(tx, get_pixel_float(x0, y0), get_pixel_float(x1, y0));
        FloatColor c1 = interp(tx, get_pixel_float(x0, y1), get_pixel_float(x1, y1));
        
        return interp(ty, c0, c1);
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

/**
 * Blend 2 images together and write the output to the first image.
 */
void blend_images(Image *img_a, Image *img_b, float factor) {
    float x_scale = img_b->width / img_a->width;
    float y_scale = img_b->height / img_a->height;
    
    for (int y = 0; y < img_a->height; y++) {
        for (int x = 0; x < img_a->width; x++) {
            float bx = x * x_scale;
            float by = y * y_scale;

            FloatColor pa = img_a->get_pixel_float(x, y);
            FloatColor pb = img_b->interp_pixel(bx, by);

            FloatColor mid = interp(factor, pa, pb);
            *(img_a->get_pixel(x, y)) = floatcolor_to_color(mid);
        }
    }
}

int main() {
    Image *img_a = read_bitmap("prog1/flowers.bmp");
    Image *img_b = read_bitmap("prog1/jar.bmp");
}
