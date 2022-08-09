#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"



// Base class for all 3d entities.
class Object3D {
public:
    Material *material;
    Object3D() : material(nullptr) {}

    virtual ~Object3D() = default;
    
    explicit Object3D(Material *material) {
        this->material = material;
    }
    virtual Vector3f getTexel(Vector3f pos) {return Vector3f(1.0, 1.0, 1.0);}
    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, double tmin) = 0;

};

#endif

