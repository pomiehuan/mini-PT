#include "mesh.hpp"
#include "sphere.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
const double MAX = std::numeric_limits<double>::max();
const double epsi = 1e-8;
bool Mesh::intersect(const Ray &r, Hit &h, double tmin) {
    // 构造包围盒
    double x1 = MAX, x2 = -MAX;
    double y1 = MAX, y2 = -MAX;
    double z1 = MAX, z2 = -MAX;
    if (r.getDirection()[0] != 0) {
        x1 = (minX - (r.getOrigin()[0])) / (r.getDirection()[0]);
        x2 = (maxX - (r.getOrigin()[0])) / (r.getDirection()[0]);
        x1 > x2 ? swap(x1,x2) : void();
    }
    if (r.getDirection()[1] != 0) {
        y1 = (minY - (r.getOrigin()[1])) / (r.getDirection()[1]);
        y2 = (maxY - (r.getOrigin()[1])) / (r.getDirection()[1]);
        y1 > y2 ? swap(y1,y2) : void();
    }
    if (r.getDirection()[2] != 0) {
        z1 = (minZ - (r.getOrigin()[2])) / (r.getDirection()[2]);
        z2 = (maxZ - (r.getOrigin()[2])) / (r.getDirection()[2]);
        z1 > z2 ? swap(z1,z2) : void();
    }
    if (max(x1, max(y1, z1)) >=  min(x2, min(y2, z2)))  
    {
        return false;
    }
    Sphere bounding_sphere = Sphere(sphereCenter, sphereRadius, material);
    Hit bshit;
    if (!bounding_sphere.intersect(r, bshit, epsi)) {  // 光线与包围球不相交
        return false;
    }
    
    // Optional: Change this brute force method into a faster one.
    bool result = false;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle.normal = n[triId];
        result |= triangle.intersect(r, h, tmin);
    }
    return result;
}

void Mesh::updateConent(const Vector3f &vec) {
    vec[0] > maxX ? maxX = vec[0] : maxX=maxX;
    vec[0] < minX ? minX = vec[0] : maxX=maxX;
    vec[1] > maxY ? maxY = vec[1] : maxY=maxY;
    vec[1] < minY ? minY = vec[1] : maxY=maxY;
    vec[2] > maxZ ? maxZ = vec[2] : maxZ=maxZ;
    vec[2] < minZ ? minZ = vec[2] : maxZ=maxZ;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {
    minX = MAX;maxX = -MAX;minY = MAX;
    maxY = -MAX;minZ = MAX;maxZ = -MAX;
    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            updateConent(vec);
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                t.push_back(trig);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                t.push_back(trig);
            }
        } else if (tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        }
    }
    computeNormal();
    computeSphere();
    f.close();
}

void Mesh::computeSphere() {
    if (v.size() == 0) {
        sphereCenter = Vector3f::ZERO;
        sphereRadius = 0.0;
        return;
    }
    // Ritter's Bounding Algorithm
    Vector3f x = v[0];
    Vector3f y = v[0];
    for (auto vertex: v) {
        if ((vertex - x).length() > (y - x).length())
            y = vertex;
    }
    Vector3f z = y;
    for (auto vertex: v) {
        if ((vertex - y).length() > (z - y).length())
            z = vertex;
    }
    sphereCenter = (y + z) / 2;
    sphereRadius = (y - z).length() / 2;
    for (auto vertex: v) {
        if ((vertex - sphereCenter).length() > sphereRadius) {
            sphereRadius = (vertex - sphereCenter).length();
        }
    }
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        // 计算法向量
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}
