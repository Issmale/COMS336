#pragma once
#include "texture.h"
#include "rtweekend.h"
#include <iostream>
#include "stb_image.h"

class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    image_texture(const char* filename) {
        int components_per_pixel;
        data = stbi_load(filename, &width, &height, &components_per_pixel, bytes_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        if (data) stbi_image_free(data);
    }

    virtual color value(double u, double v, const point3& p) const override {
        if (!data) return color(0, 1, 1);
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);
        int i = static_cast<int>(u * width);
        int j = static_cast<int>(v * height);
        if (i >= width) i = width - 1;
        if (j >= height) j = height - 1;
        const double scale = 1.0 / 255.0;
        unsigned char* pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
        return color(scale * pixel[0], scale * pixel[1], scale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};