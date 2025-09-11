#include <iostream>
#include"vec3.h"
#include"color.h"

int main() {
    std::cerr << "Program started!" << std::endl;
    const int image_width  = 256;
    const int image_height = 256;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double r = double(i) / (image_width  - 1);
            double g = double(j) / (image_height - 1);
            double b = 0.25;
            color pixel(r, g, b);
            write_color(std::cout, pixel);
        }
    }
}