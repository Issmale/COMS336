#pragma once
#include <iostream>
#include "vec3.h"

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    double scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    r = sqrt(r);
    g = sqrt(g);
    b = sqrt(b);

    int ir = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    int ig = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    int ib = static_cast<int>(256 * clamp(b, 0.0, 0.999));

    out << ir << ' ' << ig << ' ' << ib << '\n';
}