#ifndef EMISSIVE_H
#define EMISSIVE_H

#include "material.h"

class emissive : public material {
public:
    emissive(const color& emit) : emit_color(emit) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        return false;
    }

    virtual color emitted() const override {
        return emit_color;
    }

private:
    color emit_color;
};

#endif