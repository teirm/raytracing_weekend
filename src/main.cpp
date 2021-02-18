// Test program to generate a ppm file

#include "../include/rtweekend.hpp"
#include "../include/vec3.hpp"
#include "../include/ray.hpp"
#include "../include/camera.hpp"
#include "../include/color.hpp"
#include "../include/hittable_list.hpp"
#include "../include/sphere.hpp"

#include <iostream>

color ray_color(const ray& r, const hittable& world, int depth) {
    
    hit_record rec;

    // if we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    if (world.hit(r, 0.001, infinity, rec)) {
        point3 target = rec.p + rec.normal + random_unit_vector();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    // unit vector y is originally on (-1,1)
    // scale it to [0,1]
    auto t = 0.5*(unit_direction.y() + 1.0);
    // linear interpolation
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main()
{
    // image 
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width   = 400;
    const int image_height  = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    camera cam;

    // render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
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
    std::cerr << "\nDone.\n";
}
