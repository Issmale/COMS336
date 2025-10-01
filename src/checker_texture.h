#pragma once
#include "texture.h"
#include "solid_color.h"
#include <memory>
#include <cmath>

class checker_texture : public texture {
public:
    checker_texture() {
        even = nullptr;
        odd  = nullptr;
    }

    checker_texture(std::shared_ptr<texture> t0, std::shared_ptr<texture> t1) {
        even = t0;
        odd  = t1;
    }

    checker_texture(color c1, color c2) {
        even = std::make_shared<solid_color>(c1);
        odd  = std::make_shared<solid_color>(c2);
    }

    virtual color value(double u, double v, const point3& p) const override {
        double s = sin(10.0 * p.x());
        double t = sin(10.0 * p.y());
        double r = sin(10.0 * p.z());

        double sines = s * t * r;

        if (sines < 0.0) {
            return odd->value(u, v, p);
        } else {
            return even->value(u, v, p);
        }
    }

private:
    std::shared_ptr<texture> odd;  
    std::shared_ptr<texture> even;
};