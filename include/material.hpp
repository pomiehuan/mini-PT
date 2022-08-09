#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include <iostream>

enum materialType {DIFF, SPEC, REFR};

class Material {
    double clamp(double a) {return a > 0 ? a : 0;}
public:
    materialType type;
    Vector3f color, reflection;
    Texture *texture;
    double refRate;

    explicit Material(materialType _type, Vector3f _color, Vector3f _emission, double _rate, Texture *t=nullptr) :
        type(_type), color(_color), reflection(_emission), refRate(_rate), texture(t) {}

    virtual ~Material() = default;

protected:
};


#endif // MATERIAL_H
