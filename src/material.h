#pragma once
#include "ray.h"
#include "hittable.h"
#include "color.h"

class material {
public:
    virtual bool scatter(
        const ray& r_in,
        const hit_record& rec,
        color& attenuation,
        ray& scattered
    ) const = 0;

    virtual color emitted() const {
        return color(0, 0, 0);
    }
};