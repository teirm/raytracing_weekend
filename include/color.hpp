// color utility functions

#ifndef COLOR_H
#define COLOR_H

#include "vec3.hpp"

#include <iostream>

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // divide the color by the number of samples and gamma-correct for gamma=2.0
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // write the translated [0, 255] value of each color component
    out << static_cast<int>(255.999 * clamp(r, 0, 0.999)) << ' '
        << static_cast<int>(255.999 * clamp(g, 0, 0.999)) << ' '
        << static_cast<int>(255.999 * clamp(b, 0, 0.999)) << '\n';
}

#endif /* COLOR_H */
