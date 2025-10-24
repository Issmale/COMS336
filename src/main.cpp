#include <iostream>
#include <cmath>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include "triangle.h"
#include "solid_color.h"
#include "checker_texture.h"
#include "image_texture.h"
#include "obj_loader.h"
#include "emissive.h"
#include "bvh.h"

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        color emitted = rec.mat_ptr->emitted();

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return emitted + attenuation * ray_color(scattered, world, depth - 1);
        return emitted;
    }

    vec3 unit_dir = unit_vector(r.direction());
    double t = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main() {
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    auto wood_tex = std::make_shared<image_texture>("../src/wood.jpg");
    auto checker_tex = std::make_shared<checker_texture>(
        color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9)
    );

    auto ground_mat = std::make_shared<lambertian>(checker_tex);
    auto center_mat = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto glass_mat = std::make_shared<dielectric>(1.5);
    auto metal_mat = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto tri_mat = std::make_shared<lambertian>(wood_tex);

    hittable_list world;
    world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100, ground_mat));
    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5, center_mat));
    world.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.5, glass_mat));
    world.add(std::make_shared<sphere>(point3(-1, 0, -1), -0.45, glass_mat));
    world.add(std::make_shared<sphere>(point3(1, 0, -1), 0.5, metal_mat));

    const std::string obj_path = "../src/models/cube.obj";
    int loaded_triangles = load_obj_as_triangles(obj_path, world, tri_mat);
    std::cerr << "Loaded triangles from OBJ: " << loaded_triangles << " (path: " << obj_path << ")\n";
    if (loaded_triangles == 0) {
        world.add(std::make_shared<triangle>(
            point3(-0.75,0.25,-0.5),
            point3(0.75,0.25,-0.5),
            point3(0.0,1.0,-1.0),
            vec2(0,0), vec2(1,0), vec2(0.5,1),
            tri_mat
        ));
    }    

    auto light_mat = std::make_shared<emissive>(color(4, 4, 4)); 
    world.add(std::make_shared<sphere>(point3(0, 2, 0), 0.5, light_mat)); 

    bvh_node bvh_tree(world);

    point3 lookfrom(3, 3, 2);
    point3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    double vfov = 20.0;
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (i + random_double()) / (image_width - 1);
                double v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, bvh_tree, max_depth); 
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
}