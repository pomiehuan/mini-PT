#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include <string>
#include <chrono>
using namespace std;
int sampleTime = 40;
const double epsi=1e-8;
struct location {
    double x;
    double y;
}
bases[4] = {location{0,0}, location{0,1}, location{1,0}, location{1,1}};

Vector3f radiance(SceneParser *scene, const Ray &ray, int depth, unsigned short *state) {
    ++ depth;
    Group *group = scene->getGroup();
    Vector3f dIn = ray.getDirection();
    Hit hit;

    if (!group->intersect(ray, hit, epsi)) 
    {
        return Vector3f::ZERO;
    }
    Material *hitMat = hit.getMaterial();
    Vector3f matColor = hitMat->color;
    double maxRate = max(matColor.x(), max(matColor.y(), matColor.z()));
    Vector3f mat_ref = hitMat->reflection;
    double t = hit.getT();
    Vector3f n = hit.getNormal(), nl = n;
    if (Vector3f::dot(n, dIn) > 0) nl = -nl;
    Vector3f point = ray.pointAtParameter(t);

    if (depth > 5) {
        if (erand48(state) >= maxRate)
        {
            return hitMat->reflection * hit.getTexture();
        }
        matColor = matColor / maxRate;
    }

    if (hitMat->type == DIFF) // 漫反射
    { 
        double angle = 2 * M_PI * erand48(state);
        double dist = sqrt(erand48(state));
        Vector3f w = nl;
        Vector3f w2=fabs(w.x()) > 0.1 ? Vector3f(0, 1, 0): Vector3f(1, 0, 0);
        Vector3f u = Vector3f::cross(w2, w).normalized();
        Vector3f v = Vector3f::cross(w, u);
        Vector3f dOut = (u * cos(angle) * dist +v * sin(angle) * dist +w * sqrt(1 - erand48(state))).normalized();

        return (mat_ref + matColor * radiance(scene, Ray(point, dOut), depth, state)) * hit.getTexture();
    } 
    else if (hitMat->type == SPEC) // 镜面反射
    { 
        Vector3f dOut = (dIn - n * 2 * Vector3f::dot(n, dIn)).normalized();
        
        return (mat_ref + matColor * radiance(scene, Ray(point, dOut), depth, state)) * hit.getTexture();
    } 
    else if (hitMat->type == REFR) // 存在折射
    { 
        Vector3f d_reflect = (dIn - n * 2 * Vector3f::dot(n, dIn)).normalized();
        Ray rReflect(point, d_reflect);
        bool inside = Vector3f::dot(n, nl) > 0;

        const double airRate = 1., matRate = hitMat->refRate;
        double refRate = inside ? airRate / matRate : matRate / airRate;

        double ddn = Vector3f::dot(dIn, nl);
        double cos2t = 1 - refRate * refRate * (1 - ddn * ddn);

        if (cos2t < 0) // 特殊情况，全反射
        {
            return (mat_ref + matColor * radiance(scene, rReflect, depth, state)) * hit.getTexture();
        }

        Vector3f dRefract = (dIn * refRate - n * (inside? 1: -1) * (ddn * refRate + sqrt(cos2t))).normalized();
        Ray rRefract(point, dRefract);

        double a = matRate - airRate, b = matRate + airRate;//计算菲涅尔项
        double R0 = a * a / b / b;
        double c = 1 - (inside ? -ddn: Vector3f::dot(dRefract, n));
        double Re = R0 + (1-R0) * pow(c, 5), Tr = 1 - Re;
        double P = 0.25 + 0.5 * Re; double RP = Re / P, TP = Tr / (1 - P);

        Vector3f res = mat_ref;
        if (depth > 2) 
        {
            erand48(state) < P ? res += matColor * radiance(scene, rReflect, depth, state) * RP : res += matColor * radiance(scene, rRefract, depth, state) * TP;
        } else 
        {
            res += Re * matColor * radiance(scene, rReflect, depth, state);
            res += Tr * matColor * radiance(scene, rRefract, depth, state);
        }
        return res * hit.getTexture();
    }
}


void rayWithDepth(Camera *camera, Ray &ray, unsigned short  *state) {
    double focal =camera->getFocal(),aperture=camera->getAperture();
    if (focal < 0 || aperture < 0) return;
    Vector3f bias = ray.getOrigin() + aperture * Vector3f(erand48(state)-0.5, erand48(state)-0.5, erand48(state)-0.5);
    ray = Ray(bias, (ray.pointAtParameter(focal) - bias).normalized());
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) 
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 4 && argc!=3) 
    {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    if(argc==4)
    {
        sampleTime=atoi(argv[3]) / 4;
    }
    auto startTime = chrono::high_resolution_clock::now();
    SceneParser scene = SceneParser(inputFile.data());
    Image image = Image(scene.getCamera()->getWidth(), scene.getCamera()->getHeight());
    for (int x = 0; x < image.Width(); x++) 
    {
        #pragma omp parallel for schedule(dynamic)      // OpenMP 
        for (int y = 0; y < image.Height(); y++)
        {
            unsigned short randState[3] = {x * y, x * x, y * y * y};
            Vector3f finalRadiance = Vector3f::ZERO;
            for (auto samp: bases)
             {
                const double sx = samp.x;
                const double sy = samp.y;
                for (int sample_cnt = 0; sample_cnt < sampleTime; sample_cnt++) {
                    double xSample, ySample, xBias, yBias;
                    double rnd1 = 2 * erand48(randState);
                    double rnd2 = 2 * erand48(randState);
                    rnd1 < 1 ? xSample = sqrt(rnd1) - 1 : xSample = 1 - sqrt(2 - rnd1);
                    rnd2 < 1 ? ySample = sqrt(rnd2) - 1 : ySample = 1 - sqrt(2 - rnd2);
                    xBias = (sx + 0.5 + xSample) / 2.0;
                    yBias = (sy + 0.5 + ySample) / 2.0;
                    Ray cam_ray = scene.getCamera()->generateRay(Vector2f(x + xBias, y + yBias));
                    rayWithDepth(scene.getCamera(), cam_ray, randState);
                    finalRadiance += radiance(&scene, cam_ray, 0, randState);
                }
            }
            image.SetPixel(x, y, finalRadiance / sampleTime / 4.);
        }
        fprintf(stderr,"\rRendering (%d spp) %5.2f%%", sampleTime * 4, 100. * x / (scene.getCamera()->getWidth() - 1)); 
    }
    auto notification_time = chrono::high_resolution_clock::now();
    auto time_elapsed = chrono::duration_cast<chrono::duration<double>>(notification_time - startTime);
    cout<<endl;
    printf("total time elapsed during render %.4lfs\n", time_elapsed);                                                                                                                                                                                                                                                                                                                
    image.SaveBMP(outputFile.data());
    cout << "Computer Graphics!" << endl;
    return 0;
}