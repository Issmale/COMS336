#pragma once
#include "hittable.h"

class sphere : public hittable {
public:
    point3 center;
    double radius;

    sphere() {}
    sphere(point3 c, double r) : center(c), radius(r) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec3 oc = r.origin() - center;
        auto a = dot(r.direction(), r.direction());
        auto half_b = dot(oc, r.direction());
        auto c = dot(oc, oc) - radius*radius;
        auto discriminant = half_b*half_b - a*c;

        if (discriminant < 0) return false;
        auto sqrt_d = sqrt(discriminant);

        auto root = (-half_b - sqrt_d) / a;
        if (root < t_min || root > t_max) {
            root = (-half_b + sqrt_d) / a;
            if (root < t_min || root > t_max)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }
};