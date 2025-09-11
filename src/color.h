#pragma once
#include <iostream>
#include "vec3.h"

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline void write_color(std::ostream& out, const color& pixel) {

    int ir = static_cast<int>(256 * clamp(pixel.x(), 0.0, 0.999));
    int ig = static_cast<int>(256 * clamp(pixel.y(), 0.0, 0.999));
    int ib = static_cast<int>(256 * clamp(pixel.z(), 0.0, 0.999));
    out << ir << ' ' << ig << ' ' << ib << '\n';
}