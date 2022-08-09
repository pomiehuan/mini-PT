#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <cmath>


class Camera {
public:
     Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH, double _focal, double _aperture) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
        this->focal = _focal;
        this->aperture = _aperture;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;
    double getFocal() const {return focal;}
    double getAperture() const {return aperture;}
    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    int width;
    int height;
    double focal; // 焦距
    double aperture; // 光圈大小
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
   
     PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, double angle, double _focal=-1, double _aperture=-1)
            : Camera(center, direction, up, imgW, imgH, _focal, _aperture) {
                this->cx=imgW/2;
                this->cy=imgH/2;
                this->fx = imgW / (2 * tan(angle / 2));
                this->fy = imgW / (2 * tan(angle / 2));
    }
    
    Ray generateRay(const Vector2f &point) override {
        Vector3f oridic= Vector3f((point[0]-cx)/fx,(cy-point[1])/fy,1).normalized();
        Matrix3f matrix= Matrix3f(horizontal,-up,direction);
        Vector3f finaldic= matrix * oridic;
        return Ray(center,finaldic);
        // 
    }
protected:
    double fx;
    double fy;
    double cx;
    double cy;
};

#endif //CAMERA_H
