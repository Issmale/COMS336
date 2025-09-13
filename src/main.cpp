#include <iostream>
#include <cmath>
#include"vec3.h"
#include"color.h"
#include"ray.h"
#include"rtweekend.h"

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double half_b = dot(oc, r.direction());
    double c = dot(oc, oc) - radius*radius;
    double discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return -1.0;
    return (-half_b - std::sqrt(discriminant)) / a;
}

color ray_color(const ray& r) {
    point3 center(0, 0, -1);
    double t = hit_sphere(center, 0.5, r);
    if (t > 0.0) {
        vec3 n = unit_vector(r.at(t) - center);
        return 0.5 * color(n.x()+1, n.y()+1, n.z()+1);
    }

    vec3 unit_dir = unit_vector(r.direction());
    double u = 0.5*(unit_dir.y() + 1.0);
    return (1.0 - u)*color(1.0, 1.0, 1.0) + u*color(0.5, 0.7, 1.0);
}

int main() {
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;

    double viewport_height = 2.0;
    double viewport_width  = aspect_ratio * viewport_height;
    double focal_length    = 1.0;

    point3 origin(0,0,0);
    vec3 horizontal(viewport_width, 0, 0);
    vec3 vertical(0, viewport_height, 0);
    point3 lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0,0,focal_length);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (i + random_double()) / (image_width  - 1);
                double v = (j + random_double()) / (image_height - 1);
                ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
                pixel_color += ray_color(r);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
}