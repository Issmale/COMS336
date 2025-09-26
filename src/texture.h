#pragma once
#include "vec3.h"
#include "rtweekend.h"

class texture {
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};