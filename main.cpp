#include <fstream>
#include <iostream>
#include "float.h"

#include "camera.hpp"
#include "hitablelist.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "vec3.hpp"

vec3 color(const ray& r, hitable* world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 &&
            rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        } else {
            return vec3(0, 0, 0);
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main() {
    int nx = 640;
    int ny = 480;
    int ns = 100;
    std::ofstream ofs("image.ppm");
    ofs << "P3\n" << nx << " " << ny << "\n255\n";

    vec3 lower_left_corner(-2.0, -1.5, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 3.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    hitable* list[4];
    // center sphere
    list[0] =
        new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.5, 0.5, 1.0)));
    // ground
    list[1] = new sphere(vec3(0, -100.5, -1), 100,
                         new lambertian(vec3(0.5, 0.5, 0.5)));
    // metal sphere 1
    list[2] = new sphere(vec3(1.2, 0, -1), 0.5, new metal(vec3(1, 1, 1), 0.0));
    // metal sphere 2
    list[3] =
        new sphere(vec3(-1.2, 0, -1), 0.5, new metal(vec3(0.4, 0.1, 0.1), 0.2));
    hitable* world = new hitable_list(list, 4);

    // camera setting
    vec3 lookfrom(5, 5, 4);
    vec3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    float vfov = 20;  // degree
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.2;
    camera cam(lookfrom, lookat, vup, vfov, float(nx) / float(ny), aperture,
               dist_to_focus);

    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]),
                       sqrt(col[2]));  // gamma corrected
            int ir = int(255.0 * col[0]);
            int ig = int(255.0 * col[1]);
            int ib = int(255.0 * col[2]);
            ofs << ir << " " << ig << " " << ib << "\n";
        }
    }
}