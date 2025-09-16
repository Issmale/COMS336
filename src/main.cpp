#include <iostream>
#include <cmath>
#include"vec3.h"
#include"color.h"
#include"ray.h"
#include"rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }

    vec3 unit_dir = unit_vector(r.direction());
    double u = 0.5*(unit_dir.y() + 1.0);
    return (1.0 - u)*color(1.0, 1.0, 1.0) + u*color(0.5, 0.7, 1.0);
}

int main() {
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    hittable_list world;
    world.add(std::make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(std::make_shared<sphere>(point3(0,-100.5,-1), 100));

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
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
}