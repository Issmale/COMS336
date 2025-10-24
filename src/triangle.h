#pragma once
#include "hittable.h"
#include "vec3.h"
#include "vec2.h"

class triangle : public hittable {
public:
    triangle() {}

    triangle(const point3& a, const point3& b, const point3& c,
             const vec2& ta, const vec2& tb, const vec2& tc,
             std::shared_ptr<material> m)
        : v0(a), v1(b), v2(c), uv0(ta), uv1(tb), uv2(tc), mat_ptr(m) {}

    triangle(const point3& a, const point3& b, const point3& c,
             std::shared_ptr<material> m)
        : v0(a), v1(b), v2(c),
          uv0(vec2(0,0)), uv1(vec2(1,0)), uv2(vec2(0,1)), mat_ptr(m) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        const double eps = 1e-8;
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 p = cross(r.direction(), e2);
        double det = dot(e1, p);
        if (fabs(det) < eps) return false;
        double invDet = 1.0 / det;

        vec3 t = r.origin() - v0;
        double u = dot(t, p) * invDet;
        if (u < 0.0 || u > 1.0) return false;

        vec3 q = cross(t, e1);
        double v = dot(r.direction(), q) * invDet;
        if (v < 0.0 || u + v > 1.0) return false;

        double t_hit = dot(e2, q) * invDet;
        if (t_hit < t_min || t_hit > t_max) return false;

        rec.t = t_hit;
        rec.p = r.at(t_hit);
        vec3 n = unit_vector(cross(e1, e2));
        rec.set_face_normal(r, n);
        rec.mat_ptr = mat_ptr;
        rec.u = uv0.x() * (1 - u - v) + uv1.x() * u + uv2.x() * v;
        rec.v = uv0.y() * (1 - u - v) + uv1.y() * u + uv2.y() * v;
        return true;
    }

    virtual bool bounding_box(aabb& output_box) const override {
        const double epsilon = 0.0001;
        
        point3 min_pt(
            fmin(fmin(v0.x(), v1.x()), v2.x()) - epsilon,
            fmin(fmin(v0.y(), v1.y()), v2.y()) - epsilon,
            fmin(fmin(v0.z(), v1.z()), v2.z()) - epsilon
        );
        point3 max_pt(
            fmax(fmax(v0.x(), v1.x()), v2.x()) + epsilon,
            fmax(fmax(v0.y(), v1.y()), v2.y()) + epsilon,
            fmax(fmax(v0.z(), v1.z()), v2.z()) + epsilon
        );
        output_box = aabb(min_pt, max_pt);
        return true;
    }     

private:
    point3 v0, v1, v2;
    vec2 uv0, uv1, uv2;
    std::shared_ptr<material> mat_ptr;
};