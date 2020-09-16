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

// This is the order of how colors are stored.
struct Color {
    unsigned char b;
    unsigned char g; 
    unsigned char r;
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
    return out;
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
    FloatColor out;
    out.r = interp(t, a.r, b.r);
    out.g = interp(t, a.g, b.g);
    out.b = interp(t, a.b, b.b);
    return out;
}

/**
 * Class for accessing pixels from a bitmap pixel array.
 */
class Image {
public:
    const int width;
    const int height;
    const int row_size;
    char * const pixel_data = NULL;

    Image(int width, int height, int pixels_size) : 
        width(width), 
        height(height), 
        row_size(((24 * width + 31) / 32) * 4),
        pixel_data(new char[pixels_size]) 
    { }

    /**
     * Returns a pointer to the pixel at the requested position.
     */
    Color *get_pixel(int x, int y) {
        return (Color*)(pixel_data + y * row_size + 3 * x);
    }

    /**
     * Convenience function for reading a pixel and converting to a float.
     */
    FloatColor get_pixel_float(int x, int y) {
        Color color = *get_pixel(x, y);
        return color_to_float(color);
    }

    /**
     * Interpolate the pixels at the given spot
     */
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
        if (pixel_data != NULL) {
            delete pixel_data;            
        }
    }
};

/**
 * Holds all the useful data from a bitmap together.
 */
struct BitmapData {
    tagBITMAPFILEHEADER file_header;
    tagBITMAPINFOHEADER info_header;
    Image *image = NULL;

    ~BitmapData() {
        if (image != NULL) {
            delete image;
        }
    }
};

struct FileOpenException : public std::exception
{
    std::string path;

	const char * what () const throw ()
    {
    	return "Error opening file";
    }
};

BitmapData read_bitmap(std::string path) {
    BitmapData out;
    out.info_header = {0};  // zeros
    
    std::ifstream file(path);  // gets auto-closed when destroyed

    if (!file.is_open()) {
        FileOpenException e;
        e.path = path;
        throw e;
    }

    // Read the file header
    file.read((char*) &out.file_header, 14);

    // Read the info header size
    file.read((char*) &out.info_header.biSize, 4);

    // Read the rest of the info header, sized based on the header size
    file.read(((char*) &out.info_header.biSize) + 4, out.info_header.biSize - 4);

    // Read the pixels
    out.image = new Image(out.info_header.biWidth, out.info_header.biHeight, out.info_header.biSizeImage);
    file.read(out.image->pixel_data, out.info_header.biSizeImage);
    return out;
}

void write_bitmap(BitmapData *data, std::string path) {
    std::ofstream file(path, std::ofstream::binary);  // gets auto-closed when destroyed

    file.write((char*) &data->file_header, 14);
    file.write((char*) &data->info_header, data->info_header.biSize);
    file.write(data->image->pixel_data, data->info_header.biSizeImage);
}

/**
 * Blend 2 images together and write the output to the first image.
 */
void blend_images(Image *img_a, Image *img_b, float factor) {
    float x_scale = (float) img_b->width / img_a->width;
    float y_scale = (float) img_b->height / img_a->height;

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

void blend_images_from_path(std::string path_a, std::string path_b, std::string path_out, float factor) {
    // Load images into memory
    BitmapData loaded_a = read_bitmap(path_a);
    BitmapData loaded_b = read_bitmap(path_b);

    // Pick the larger and smaller images
    BitmapData *img_larger;
    BitmapData *img_smaller;
    if (loaded_a.info_header.biWidth > loaded_b.info_header.biWidth) { 
        img_larger = &loaded_a;
        img_smaller = &loaded_b;
    } else {
        img_larger = &loaded_b;
        img_smaller = &loaded_a;
    }

    // Blend together and write
    blend_images(img_larger->image, img_smaller->image, factor);
    write_bitmap(img_larger, path_out);
}

void print_help_text(std::string exec) {
    std::cout << "Usage: " << exec << " <first> <second> <factor> <output>" << std::endl << std::endl
              << "  first, second       the input images" << std::endl
              << "  factor              a number describing how much of each to blend together, " << std::endl
              << "                      preferably within [0, 1]. Use factors outside of [0, 1] " << std::endl
              << "                      at your own risk." << std::endl
              << "  output              where to write the output" << std::endl;
}

int main(int argc, char *argv[]) {
    std::string exec = argv[0];
    if (argc != 5) {
        std::cout << "Error: Invalid arguments." << std::endl << std::endl;
        print_help_text(exec);
        return 1;
    }
    std::string path_a = argv[1];
    std::string path_b = argv[2];
    float factor;
    try {
        factor = std::stof(argv[3]);
    } catch (std::invalid_argument& e) {
        std::cout << "Error: Invalid factor. Factor must be a decimal." << std::endl << std::endl;
        print_help_text(exec);
        return 2;
    }
    std::string path_out = argv[4];

    try {
        blend_images_from_path(path_a, path_b, path_out, factor);
    } catch (FileOpenException &e) {
        std::cout << "Error: Could not open file " << e.path << std::endl << std::endl;
        print_help_text(exec);
        return 3;
    }
    return 0;
}
