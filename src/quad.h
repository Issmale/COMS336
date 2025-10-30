#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "vec3.h"

class quad : public hittable {
public:
    quad() {}
    
    quad(point3 _min, point3 _max, std::shared_ptr<material> m, int axis_type = 2)
        : box_min(_min), box_max(_max), mat_ptr(m), axis(axis_type) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        
        int a_axis, b_axis;
        if (axis == 0) { a_axis = 1; b_axis = 2; }
        else if (axis == 1) { a_axis = 0; b_axis = 2; }
        else { a_axis = 0; b_axis = 1; }

        double k = (axis == 0) ? box_min.x() : (axis == 1) ? box_min.y() : box_min.z();
        

        double ray_dir_component = (axis == 0) ? r.direction().x() : 
                                   (axis == 1) ? r.direction().y() : r.direction().z();
        double ray_orig_component = (axis == 0) ? r.origin().x() : 
                                    (axis == 1) ? r.origin().y() : r.origin().z();
        
        if (fabs(ray_dir_component) < 1e-8)
            return false;

        double t = (k - ray_orig_component) / ray_dir_component;
        if (t < t_min || t > t_max)
            return false;

        point3 p = r.at(t);
        double p_a = (a_axis == 0) ? p.x() : (a_axis == 1) ? p.y() : p.z();
        double p_b = (b_axis == 0) ? p.x() : (b_axis == 1) ? p.y() : p.z();
        
        double min_a = (a_axis == 0) ? box_min.x() : (a_axis == 1) ? box_min.y() : box_min.z();
        double max_a = (a_axis == 0) ? box_max.x() : (a_axis == 1) ? box_max.y() : box_max.z();
        double min_b = (b_axis == 0) ? box_min.x() : (b_axis == 1) ? box_min.y() : box_min.z();
        double max_b = (b_axis == 0) ? box_max.x() : (b_axis == 1) ? box_max.y() : box_max.z();

        if (p_a < min_a || p_a > max_a || p_b < min_b || p_b > max_b)
            return false;

        rec.t = t;
        rec.p = p;
        
        vec3 outward_normal;
        if (axis == 0) outward_normal = vec3(1, 0, 0);
        else if (axis == 1) outward_normal = vec3(0, 1, 0);
        else outward_normal = vec3(0, 0, 1);
        
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.u = (p_a - min_a) / (max_a - min_a);
        rec.v = (p_b - min_b) / (max_b - min_b);

        return true;
    }

    virtual bool bounding_box(aabb& output_box) const override {
        const double epsilon = 0.0001;
        point3 pad_min = box_min - vec3(epsilon, epsilon, epsilon);
        point3 pad_max = box_max + vec3(epsilon, epsilon, epsilon);
        output_box = aabb(pad_min, pad_max);
        return true;
    }

private:
    point3 box_min, box_max;
    std::shared_ptr<material> mat_ptr;
    int axis;
};

#endif