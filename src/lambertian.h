#pragma once
#include "material.h"
#include "texture.h"
#include "solid_color.h"

class lambertian : public material {
public:
    lambertian(const color& c) {
        albedo = std::make_shared<solid_color>(c);
    }

    lambertian(std::shared_ptr<texture> tex) {
        albedo = tex;
    }

    virtual bool scatter(
        const ray& r_in,             
        const hit_record& rec,      
        color& attenuation,          
        ray& scattered               
    ) const override 
    {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);

        attenuation = albedo->value(rec.u, rec.v, rec.p);

        return true;
    }

private:
    std::shared_ptr<texture> albedo;
};