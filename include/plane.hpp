#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>


class Plane : public Object3D {
public:
    Plane() {
        normal = Vector3f(1.0, 0.0, 0.0);
        D = 0;
    }

    Plane(const Vector3f &normal, double d, Material *m) : Object3D(m) {
        this->D=-d;
        this->normal=normal;
        this->theplane=m;
    }

    ~Plane() override = default;
    bool intersect(const Ray &r, Hit &h, double tmin) override {
        double down=Vector3f::dot(normal,r.getDirection());
        if (down==0)
        {
            return false;
        }
        double t=(-(D+Vector3f::dot(normal,r.getOrigin())))/down;
        if (t >0 )
        {
            if( h.getT()>=t &&t >  tmin)
            {
                h.set(t,theplane,normal);
                return true;
            }
        }
        return false;
    }

protected:
    Material * theplane;
    Vector3f normal;
    double D;

};

class Rectangle : public Object3D {
public:

    Vector3f point;
    Vector3f x, y;
    Vector3f normal;
    Rectangle(const Vector3f &_point, const Vector3f &_x, const Vector3f &_y, Material *m) :
        point(_point), x(_x), y(_y), Object3D(m) {
        normal = Vector3f::cross(x, y).normalized();
    }
    virtual ~Rectangle() override = default;
    
    Vector3f getTexel(Vector3f pos) override {
        assert (fabs(Vector3f::dot(pos - point, normal)) < 1e-8);
        double xlength = x.length();
        double ylength = y.length();
        double ratiro =min(material->texture->getwidth() / xlength, material->texture->getheight() / ylength);
        int u = ratiro * Vector3f::dot(pos-point, x) /  xlength;
        int v = ratiro * Vector3f::dot(pos-point, y) / ylength;
        return material->texture->getTexel(u, v);
    }

    bool intersect(const Ray &ray, Hit &h, double tmin) override {
        Plane *plane = new Plane(normal, Vector3f::dot(normal, point), material);
        Hit hit = h;
        if (!plane->intersect(ray, hit, tmin)) {
            delete plane;
            return false;
        }
        delete plane;
        Vector3f hitPoint = ray.pointAtParameter(hit.getT());
        Vector3f remotePoint = point + x + y;
        if (point[0] <= hitPoint[0] && hitPoint[0] <= remotePoint[0] &&
            point[1] <= hitPoint[1] && hitPoint[1] <= remotePoint[1] &&
            point[2] <= hitPoint[2] && hitPoint[2] <= remotePoint[2]) 
            {
                h.set(hit.getT(), hit.getMaterial(), hit.getNormal(), getTexel(hitPoint));
                return true;
            }
        return false;
    }
protected:
};


#endif //PLANE_H
