#pragma once
#include <stdlib.h>
#include <limits>
#include <memory>
#include <random>
#include "vec3.h"

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), random_double(-1,1));
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max+1));
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_vec3() {
    return vec3(random_double(), random_double(), random_double());
}

inline vec3 random_vec3(double min, double max) {
    return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
}