#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        
    }

    Sphere(const Vector3f &_center, double _radius, Material *material)
        : center(_center), radius{_radius}, Object3D(material) {}

    Sphere(Sphere *sphere) : Sphere(sphere->center, sphere->radius, sphere->material) {}

    ~Sphere() override = default;
    bool intersect(const Ray &r, Hit &hit, double tmin) override {

        Vector3f l = center - r.getOrigin();
        double dirLength = r.getDirection().length();
        double tp = Vector3f::dot(l, r.getDirection().normalized());
        if (tp < tmin) 
        {
            return false;
        }
        double d2 = pow(l.length(), 2) - tp * tp;
        if (d2 > radius * radius) 
        {
            return false;
        }
        double t2 = radius * radius - d2;
        double t = tp - sqrt(t2);
        t =t / dirLength;
        if(t > tmin && t< hit.getT())
        {
            Vector3f inter_n = (r.pointAtParameter(t) - center).normalized();
            hit.set(t, material, inter_n, getTexel(r.pointAtParameter(t)));
            return true;
        }
        return false;
    }

    Vector3f getTexel(Vector3f position) override {
        if (material->texture == nullptr) 
        {
            return Vector3f(1.0, 1.0, 1.0);
        } 
        Vector3f direction = (position - center).normalized();
        Vector3f hor(0, 0, 1), ver(0, 1, 0);
        double dleta = M_PI - acos(Vector3f::dot(ver, direction));
        Vector3f temp = direction;
        temp[1] = 0;
        direction = temp.normalized();
        
        double phi = M_PI - acos(Vector3f::dot(hor, direction));
        if (Vector3f::dot(ver, Vector3f::cross(hor, direction)) < 0) 
        {
            phi = 2 * M_PI - phi;
        }
        phi = 2 * M_PI - phi;

        int u = (material->texture->getwidth()-1) * (phi / 2 / M_PI);
        int v = (material->texture->getheight()-1) * (dleta / M_PI);
        return material->texture->getTexel(u, v);
    }


protected:
    Vector3f center;
    double radius;
};


#endif
