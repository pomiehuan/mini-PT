#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        texture = Vector3f(1.0, 1.0, 1.0);
        t = 1e38;
    }

    Hit(double _t, Material *m, const Vector3f &n, const Vector3f &_texture = Vector3f(1, 1, 1)) {
        t = _t;
        material = m;
        normal = n;
        texture = _texture;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        texture = h.texture;
    }

    // destructor
    ~Hit() = default;

    double getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    const Vector3f &getTexture() const {
        return texture;
    }

    void set(double _t, Material *m, const Vector3f &n, Vector3f _texture=Vector3f(1.,1.,1.)) {
        t = _t;
        material = m;
        normal = n;
        texture = _texture;
    }

private:
    double t;
    Material *material;
    Vector3f normal;
    Vector3f texture;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
