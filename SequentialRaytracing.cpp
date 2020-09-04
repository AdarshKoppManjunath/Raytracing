/* 

 File: SequentialRaytracing.cpp 
 Project: Ray Tracing
 License: https://en.wikipedia.org/wiki/WTFPL
 Author: Modified By Adarsh Koppa Manjunath
 Date: July 30, 2020
 Description: 
                This program is wriiten for ray tracing in sequential.
                Program is referred from https://github.com/ssloy, modified by me as per requirements.
                Below are the four steps mainly considered.
                1) Decalring scene, object and lights.
                2) Create a scene and image buffer.
                3) Apply ray tracing algorithm for each pixel ( not parallelized ).
                4) Copy the image array to .ppm file.

                I have also referred below links for this program. 
                https://code.google.com/archive/p/simple-ray-tracing/ 
                http://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-Sphere-Intersection.html
                https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
                https://github.com/marczych/RayTracer
 */ 

#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "geometry.h"
#include <sys/time.h>
#include <time.h>


double timef_()
{

    /* This function gives the current time in msec
    Source: https://code.google.com/archive/p/simple-ray-tracing/downloads
    */ 
    double msec;
    struct timeval tv;
    gettimeofday(&tv, 0);
    msec = tv.tv_sec * 1.0e3 + tv.tv_usec * 1.0e-3;
    return msec;
}

float second_()
{
    return (float)clock() / CLOCKS_PER_SEC;
}


// for lights

struct Light {
    Light(const Vec3f& p, const float i) : position(p), intensity(i) {}
    Vec3f position;
    float intensity;
};


// for materials 

struct objecttype {
    objecttype(const float r, const Vec4f& a, const Vec3f& color, const float spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
    objecttype() : refractive_index(1), albedo(1, 0, 0, 0), diffuse_color(), specular_exponent() {}
    float refractive_index;
    Vec4f albedo;
    Vec3f diffuse_color;
    float specular_exponent;
};

// for sphere

struct Sphere {

    Vec3f center;
    float radius;
    objecttype object;

    Sphere(const Vec3f& c, const float r, const objecttype& m) : center(c), radius(r), object(m) {}

    bool ray_intersect(const Vec3f& orig, const Vec3f& direction, float& t0) const {

        /* We know the sphere center Center value, ray origin (orig)  and direction(direction) value, we need to find out whether ray will intersect the sphere or not.
           to find out the intersection, we have the below formula
           Origin + Direction * t= Point

           In this, we need to find t.

           By finding  t0 and t1 ( for two points of intersection) , we can find intersection points

           Referred from: http://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-Sphere-Intersection.html

       */

        Vec3f L = center - orig;  //vector  from origin to center
        float tca = L * direction;         //tca is the distance from the origin to the a point on the ray halfway between the 2 intersection points
        float d2 = L * L - tca * tca;  //d2 = √(tca² - L²) d is the distance from center of the sphere to ray, calucalted using simple Pythagoras’ Theorem.
        if (d2 > radius * radius) return false;  // there is no intersection we cam skip the furthure steps.
        float thc = sqrtf(radius * radius - d2);  //Using Pythagoras’ Theorem, we can calucalucate thc, as we already know the radius and d, thc = √(radius² - d2²)

        // t0 and t1 are the intersection points, if both of them is zero that means there is no interaction, we can return false. If anyone of them is non zero that means there is an intersection.

        t0 = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }
};

Vec3f reflect(const Vec3f& I, const Vec3f& N) {
    /* This function is used to caluclate reflection
       Referred from- https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel 
    */
    return I - N * 2.f * (I * N);
}

Vec3f refract(const Vec3f& I, const Vec3f& N, const float eta_t, const float eta_i = 1.f) {

    /* The calculation of the refracted ray  with Snell’s law
       Referred from- https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel 
    */

    float cosi = -std::max(-1.f, std::min(1.f, I * N));
    if (cosi < 0) return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
    float eta = eta_i / eta_t;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? Vec3f(1, 0, 0) : I * eta + N * (eta * cosi - sqrtf(k)); // k<0 = total reflection, no ray to refract.
}


bool scene_intersect(const Vec3f& orig, const Vec3f& direction, const std::vector<Sphere>& spheres, Vec3f& hit, Vec3f& N, objecttype& object) {

    /*
        Step 1: This function is used to find intersection point, we wil walk through each pixel of every sphere and check wethere it will intersect with the ray or not.
                To calucalte intersection points we need to calucalte 't' in the below formula.
                Origin + Direction * t= Point  where point gives intersection.
                We send each  pixel of every sphere along with the direction and dist_i variable to ray interesect function.
                Return result gives the value for dist_i which is  t in the above formula.

        Step 2:  introduce a suface plane and consider intersection points as well.

       Referred from: https://github.com/ssloy/tinyraytracer

    */

    float spheres_dist = std::numeric_limits<float>::max();
    for (size_t i = 0; i < spheres.size(); i++) {
        float dist_i;
        if (spheres[i].ray_intersect(orig, direction, dist_i) && dist_i < spheres_dist) { // send pixel, direction and dist to ray_intersect function.
            spheres_dist = dist_i;
            hit = orig + direction * dist_i; //caluclate intersection point  Origin + Direction * t= Point
            N = (hit - spheres[i].center).normalize(); //intersection point normalized
            object = spheres[i].object;
        }
    }

    /* below piece of code is to introduce a suface plane*/
    float plain_surface_dist = std::numeric_limits<float>::max();
    if (fabs(direction.y) > 1e-3) {
        float d = -(orig.y + 4) / direction.y; // the plain suface has equation y = -4
        Vec3f pt = orig + direction * d;
        if (d > 0 && fabs(pt.x) < 10 && pt.z<-10 && pt.z>-30 && d < spheres_dist) {
            plain_surface_dist = d; //caluclate intersection point  Origin + Direction * t= Point
            hit = pt;
            N = Vec3f(0, 1, 0);
            object.diffuse_color = (int(.5 * hit.x + 1000) + int(.5 * hit.z)) & 1 ? Vec3f(.3, .2, .1) : Vec3f(.3, .2, .1);
        }
    }
    return std::min(spheres_dist, plain_surface_dist) < 1000;
}

Vec3f cast_ray(const Vec3f& orig, const Vec3f& direction, const std::vector<Sphere>& spheres, const std::vector<Light>& lights, size_t depth = 0) {

    /* In this function scene intersection and different lighting values will be considered including, diffuse, specualr, shadow, releftion and refraction.
       Referred from: https://github.com/ssloy/tinyraytracer  
    */

    Vec3f point, N;
    objecttype object;
    vec<3, float> diffuse, specular, reflection, refraction;

    // Maximum number of light reflections- currently considered depth as 8
    if (depth > 4 || !scene_intersect(orig, direction, spheres, point, N, object)) {
        return Vec3f(0.1, 0.4, 0.5); // background color
    }

    //Refraction
    Vec3f reflect_direction = reflect(direction, N).normalize();
    Vec3f refract_direction = refract(direction, N, object.refractive_index).normalize();
    Vec3f refract_orig = refract_direction * N < 0 ? point - N * 1e-3 : point + N * 1e-3;


    //Reflection
    Vec3f reflect_orig = reflect_direction * N < 0 ? point - N * 1e-3 : point + N * 1e-3; // offset the original point to avoid occlusion by the object itself
    Vec3f reflect_color = cast_ray(reflect_orig, reflect_direction, spheres, lights, depth + 1);
    Vec3f refract_color = cast_ray(refract_orig, refract_direction, spheres, lights, depth + 1);



    float diffuse_light_intensity = 0, specular_light_intensity = 0;

    for (size_t i = 0; i < lights.size(); i++) {
        Vec3f light_direction = (lights[i].position - point).normalize();
        float light_distance = (lights[i].position - point).norm();


        /*Shadows are incorporated into lighting.
        To determine if a light source should contribute to the lighting at an intersection point a shadow ray is cast from the intersection point to the light source.
        If there is an intersection before the light source then this point is in the shadow of that light source.*/
        Vec3f shadow_orig = light_direction * N < 0 ? point - N * 1e-3 : point + N * 1e-3; // checking if the point lies in the shadow of the lights.
        Vec3f shadow_pt, shadow_N;
        objecttype tmpmaterial;
        if (scene_intersect(shadow_orig, light_direction, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance)
            continue;

        diffuse_light_intensity += lights[i].intensity * std::max(0.f, light_direction * N); //to caluclate diffuse
        specular_light_intensity += powf(std::max(0.f, -reflect(-light_direction, N) * direction), object.specular_exponent) * lights[i].intensity; //to caluclate specualr
    }

    /*Diffuse lighting - determined by computing the intensity of the light at a point on the sphere
      Specular lighting -  calculated by computing a reflection ray by reflecting the light vector about the normal at the intersection point.
      Reflections- are performed by casting rays originating from the intersection point directed along the reflection vector
      Refractions- occur when rays intersect refractive spheres.The light at the intersection point is determined by blending the reflectedand refracted light at that point.*/

    diffuse = object.diffuse_color * diffuse_light_intensity * object.albedo[0];
    specular = Vec3f(1., 1., 1.) * specular_light_intensity * object.albedo[1];
    reflection = reflect_color * object.albedo[2];
    refraction = refract_color * object.albedo[3];

    return    diffuse + specular + reflection + refraction;
}



int main() {
    /*
      1) In this function, at very first we declared all the objects ( mostly spheres) and lights.
      2) We will create an image of required height and width, and create a 1d buffer for the same
      3) We will walk through every pixel or array element and apply ray tracing algorithm ( sequential )
      4) Finally, computed 1d array is copied to .ppm file.
   */

   /*Step 1- All the obejcts considered in this raytracing are mostly spheres with different color, size, postion and opacity level.*/

    objecttype     gray_sphere(1.0, Vec4f(0.6, 0.3, 0.1, 0.0), Vec3f(0.4, 0.4, 0.3), 50.);
    objecttype     violet_sphere(1.0, Vec4f(0.9, 0.1, 0.0, 0.0), Vec3f(0.2, 0.1, 0.3), 10.);
    objecttype     left_mirror(1.0, Vec4f(0.0, 10.0, 0.8, 0.0), Vec3f(1.0, 1.0, 1.0), 1425.);
    objecttype     right_mirror(1.0, Vec4f(0.0, 10.0, 0.8, 0.0), Vec3f(1.0, 1.0, 1.0), 1425.);
    objecttype     transparent_sphere_1(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.3, 0.8), 75.);
    objecttype     transparent_sphere_2(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.3, 0.8), 75.);
    objecttype     transparent_sphere_3(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.3, 0.8), 75.);
    objecttype     transparent_sphere_4(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.3, 0.8), 75.);
    objecttype     transparent_sphere_5(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.3, 0.8), 75.);
    objecttype     transparent_sphere_6(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.3, 0.8), 75.);
    objecttype     transparent_sphere_7(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.2, 0.8), 75.);



    std::vector<Sphere> spheres;

    //declare sphere center, radius and object type.

    spheres.push_back(Sphere(Vec3f(-3, 0, -16), 2, gray_sphere));
    spheres.push_back(Sphere(Vec3f(1.5, -0.5, -18), 3, violet_sphere));
    spheres.push_back(Sphere(Vec3f(-8, 5, -17), 4, left_mirror));
    spheres.push_back(Sphere(Vec3f(7, 5, -18), 4, right_mirror));
    spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, transparent_sphere_1));
    spheres.push_back(Sphere(Vec3f(1.3, -2.5, -9), 0.85, transparent_sphere_2));
    spheres.push_back(Sphere(Vec3f(-3.1, -2.5, -9), 0.85, transparent_sphere_3));
    spheres.push_back(Sphere(Vec3f(3.0, -2.5, -9), 0.85, transparent_sphere_4));
    spheres.push_back(Sphere(Vec3f(-4.30, -2.5, -9), 0.85, transparent_sphere_5));
    spheres.push_back(Sphere(Vec3f(4.67, -2.5, -9), 0.85, transparent_sphere_6));
    spheres.push_back(Sphere(Vec3f(-5.70, -2.5, -9), 0.85, transparent_sphere_7));


    std::vector<Light>  lights;

    /*Step 2: declare all lights position and intensity level.*/

    lights.push_back(Light(Vec3f(-20, 20, 20), 1.5));
    lights.push_back(Light(Vec3f(30, 50, -25), 1.8));
    lights.push_back(Light(Vec3f(30, 20, 30), 1.7));

    std::cout << "Squential RayTracing is Running...." << std::endl;

    double t_start, t_end; //start and end time 



    /*Step 3: Create an image buffer of required height and width, and apply ray tracing algorithm*/


    t_start = timef_() / 1000.0; //considering time right before starting ray tracing alogorithm

    std::cout << "Raytracing Algorithm Started..........." << std::endl;

    const int   rows = 1024; //image width
    const int   cols = 768;  //image height
    const float fov = M_PI / 3.; // M_PI is pi value
    std::vector<Vec3f> image(rows * cols); //1d buffer for image array.
                                           
                                           
    //Apply ray tracing algorithm on every pixel of the image array ( not parallelized ).

    for (size_t j = 0; j < cols; j++) { // actual rendering loop
        for (size_t i = 0; i < rows; i++) {
            float directionX = (i + 0.5) - rows / 2.;
            float directionY = -(j + 0.5) + cols / 2.;    // this flips the image at the same time
            float directionZ = -cols / (2. * tan(fov / 2.));
            image[i + j * rows] = cast_ray(Vec3f(0, 0, 0), Vec3f(directionX, directionY, directionZ).normalize(), spheres, lights);
        }
    }

    t_end = timef_() / 1000.0; //time after finishing the ray tracing algorithm.
    std::cout << "Raytracing Algorithm Finished......" << std::endl;

    std::cout << "Time required to run ray tracing algorithm by squential  program is ....." << t_end - t_start << " seconds." << std::endl;



    /* Step 4: After computing the values for every pixel, we are copying it to the ppm file ( this is not been parallilezed) */

    std::ofstream file_pointer;
    file_pointer.open("./SquentialRaytracing.ppm", std::ios::binary);  //image array to out.ppm file
    file_pointer << "P6\n" << rows << " " << cols << "\n255\n"; //.ppm format needs row and col at the top, we are the same in the out.ppm file


    //we will walk through every pixel and scale it in the range 0 to 255 and copy it to the out.ppm file.

    for (size_t i = 0; i < cols * rows; ++i) {
        Vec3f& c = image[i];
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max > 1) c = c * (1. / max);
        for (size_t j = 0; j < 3; j++) {
            file_pointer << (char)(255 * std::max(0.f, std::min(1.f, image[i][j])));
        }
    }

    file_pointer.close(); //colse the file


    return 0;

}