#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#ifdef _OPENMP
#include <omp.h> 
#endif
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
#include "moving_sphere.h"
#include "perlin.h"
#include "noise_texture.h"
#include "quad.h"
#include "translate.h"

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

void write_rgbe(std::ofstream& out, float r, float g, float b) {
    float v = std::max({r, g, b});
    if (v < 1e-32f) {
        out.put(0); out.put(0); out.put(0); out.put(0);
        return;
    }
    int e;
    float m = std::frexp(v, &e) * 256.0f / v;
    out.put(static_cast<unsigned char>(r * m));
    out.put(static_cast<unsigned char>(g * m));
    out.put(static_cast<unsigned char>(b * m));
    out.put(static_cast<unsigned char>(e + 128));
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

    auto light_mat = std::make_shared<emissive>(color(8, 8, 8));
    world.add(std::make_shared<sphere>(point3(0, 3, -1), 0.5, light_mat));

    auto moving_mat = std::make_shared<lambertian>(color(0.7, 0.3, 0.3));
    world.add(std::make_shared<moving_sphere>(
        point3(-0.5, 0.5, -1.0),
        point3(0.5, 0.5, -1.0),
        0.0, 1.0,
        0.25,
        moving_mat
    ));

    auto noise_tex = std::make_shared<noise_texture>(4.0);
    auto noise_mat = std::make_shared<lambertian>(noise_tex);
    world.add(std::make_shared<sphere>(point3(1.5, 0.5, -1), 0.5, noise_mat));

    auto wall_mat = std::make_shared<lambertian>(color(0.8, 0.2, 0.2));
    world.add(std::make_shared<quad>(
        point3(-2, -1, -3),
        point3(2, 2, -3),
        wall_mat,
        2
    ));

    auto instanced_mat = std::make_shared<lambertian>(color(0.2, 0.8, 0.2));
    auto instanced_sphere = std::make_shared<sphere>(point3(0, 0.5, -2), 0.3, instanced_mat);

    world.add(instanced_sphere);

    world.add(std::make_shared<translate>(instanced_sphere, vec3(1, 0, 0)));
    world.add(std::make_shared<translate>(instanced_sphere, vec3(-1, 0, 0)));
    world.add(std::make_shared<translate>(instanced_sphere, vec3(0, 1, 0)));

    bvh_node bvh_tree(world);

    point3 lookfrom(3, 3, 2);
    point3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    double vfov = 40.0;
    double dist_to_focus = 3.7;
    double aperture = 0.05;
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);       

    std::vector<std::vector<color>> framebuffer(image_height, std::vector<color>(image_width));
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    std::vector<std::vector<int>> sample_counts(image_height, std::vector<int>(image_width, 0));
    const int min_samples = 30;
    const double variance_threshold = 0.001;

    #pragma omp parallel for schedule(dynamic)
    for (int j = image_height - 1; j >= 0; --j) {
    #pragma omp critical
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            double sum_r_sq = 0, sum_g_sq = 0, sum_b_sq = 0;
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (i + random_double()) / (image_width - 1);
                double v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                color sample = ray_color(r, bvh_tree, max_depth);
                pixel_color += sample;
                sum_r_sq += sample.x() * sample.x();
                sum_g_sq += sample.y() * sample.y();
                sum_b_sq += sample.z() * sample.z();
                sample_counts[j][i]++;
                if (s >= 30 && s % 10 == 0) {
                    double n = s + 1;
                    color mean = pixel_color / n;
                    double var_r = (sum_r_sq / n) - mean.x() * mean.x();
                    double var_g = (sum_g_sq / n) - mean.y() * mean.y();
                    double var_b = (sum_b_sq / n) - mean.z() * mean.z();
                    double max_std = std::sqrt(std::max({var_r, var_g, var_b}));
                    if (max_std / std::sqrt(n) < 0.001) {
                        break;
                    }
                }
            }
            framebuffer[j][i] = pixel_color;
        }
    }    

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            write_color(std::cout, framebuffer[j][i], sample_counts[j][i]);
        }
    }
    std::ofstream hdrfile("output.hdr", std::ios::binary);
    hdrfile << "#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n-Y " << image_height << " +X " << image_width << "\n";

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            color c = framebuffer[j][i] / static_cast<float>(sample_counts[j][i]);
            write_rgbe(hdrfile, c.x(), c.y(), c.z());
        }
    }
    hdrfile.close();
    std::cerr << "\nDone.\n";
}