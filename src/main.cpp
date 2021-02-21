// Test program to generate a ppm file

#include "../include/rtweekend.hpp"
#include "../include/vec3.hpp"
#include "../include/ray.hpp"
#include "../include/camera.hpp"
#include "../include/color.hpp"
#include "../include/hittable_list.hpp"
#include "../include/sphere.hpp"
#include "../include/material.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

using color_vector = std::vector<color>;

color ray_color(const ray& r, const hittable& world, int depth) {
    
    hit_record rec;

    // if we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth-1);
        }
        return color(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    // unit vector y is originally on (-1,1)
    // scale it to [0,1]
    auto t = 0.5*(unit_direction.y() + 1.0);
    // linear interpolation
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz   = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    
    return world;
}

void generate_image(const int image_height,
                    const int image_width, 
                    const int start_height,
                    const int end_height,
                    const int samples_per_pixel, 
                    const camera &cam,
                    const hittable_list& world)
{
    const int max_depth = 50;
    
    for (int j = start_height-1; j >= end_height; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
}

int main()
{
    using namespace std::literals;

    // image 
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width   = 1200;
    const int image_height  = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 2;
    
    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3   vup(0, 1, 0);
    auto   dist_to_focus = 10.0; 
    auto   aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // start timer 
    const auto start = std::chrono::steady_clock::now();

    // world
    auto world = random_scene();

    
    auto concurrency = std::thread::hardware_concurrency();
    if (concurrency == 0) {
        /* unable to fetch on platform */
        concurrency = 2;
        std::cerr << "Unable to fetch hardware concurrency. Concurrency set to " << concurrency << "\n";
    }

    auto height_partition_size = image_height / concurrency;  

    // render
    int start_height = image_height;
    int end_height = image_height - height_partition_size;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (unsigned i = 0; i < concurrency - 1; ++i) {
        std::cerr << "Rendering on [" << end_height << "," << start_height << ")\n";
        generate_image(image_height, image_width, start_height, end_height, samples_per_pixel, cam, world);
        start_height = end_height;
        end_height = end_height - height_partition_size;
    }
    
    // do final range if uneven division of height
    std::cerr << "Rendering on [" << 0 << "," << start_height << ")\n";
    generate_image(image_height, image_width, start_height, 0, samples_per_pixel, cam, world);

    // end timer
    const auto end = std::chrono::steady_clock::now();
    std::cerr << "\nDone. Elapsed Time: "
              << (end - start) / 1s << "s.\n";
}
