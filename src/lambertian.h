#pragma once
#include "material.h"
#include "texture.h"
#include "solid_color.h"
#include "rtweekend.h"

inline vec3 random_cosine_direction() {
    double r1 = random_double();
    double r2 = random_double();
    double z = sqrt(1.0 - r2);
    
    double phi = 2.0 * pi * r1;
    double x = cos(phi) * sqrt(r2);
    double y = sin(phi) * sqrt(r2);
    
    return vec3(x, y, z);
}

inline void onb_from_w(const vec3& w, vec3& u, vec3& v) {
    vec3 a = (fabs(w.x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    v = unit_vector(cross(w, a));
    u = cross(w, v);
}

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
        vec3 u, v;
        onb_from_w(rec.normal, u, v);
        
        vec3 local_dir = random_cosine_direction();
        
        vec3 scatter_direction = local_dir.x() * u + 
                                 local_dir.y() * v + 
                                 local_dir.z() * rec.normal;
        
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, unit_vector(scatter_direction));
        attenuation = albedo->value(rec.u, rec.v, rec.p);

        return true;
    }

private:
    std::shared_ptr<texture> albedo;
};