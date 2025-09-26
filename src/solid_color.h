#pragma once
#include "texture.h"

class solid_color : public texture {
public:
    solid_color() {}

    solid_color(color c) {
        color_value = c;
    }

    solid_color(double r, double g, double b) {
        color_value = color(r, g, b);
    }

    virtual color value(double u, double v, const point3& p) const override {
        return color_value;
    }

private:
    color color_value; 
};