#pragma once
#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
public:
    triangle() {}

    triangle(const point3& a, const point3& b, const point3& c,
             std::shared_ptr<material> material)
        : v0(a), v1(b), v2(c), mat_ptr(material) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        const double EPS = 1e-8;

        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;

        vec3 pvec = cross(r.direction(), edge2);
        double det = dot(edge1, pvec);

        if (fabs(det) < EPS) {
            return false;
        }

        double invDet = 1.0 / det;

        vec3 tvec = r.origin() - v0;

        double u = dot(tvec, pvec) * invDet;
        if (u < 0.0 || u > 1.0) {
            return false;
        }

        vec3 qvec = cross(tvec, edge1);
        double v = dot(r.direction(), qvec) * invDet;
        if (v < 0.0 || (u + v) > 1.0) {
            return false;
        }

        double t = dot(edge2, qvec) * invDet;
        if (t < t_min || t > t_max) {
            return false;
        }

        rec.t = t;
        rec.p = r.at(t);
        vec3 normal = unit_vector(cross(edge1, edge2));
        rec.set_face_normal(r, normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

private:
    point3 v0, v1, v2;
    std::shared_ptr<material> mat_ptr;
};