#ifndef GROUP_H
#define GROUP_H
#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>
using namespace std;

// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {
        objects.clear();
    }

    explicit Group (int num_objects) : Object3D() {
        objects.clear();
       
    }

    ~Group() override {
        
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        bool ifintersect=false;
        for (auto i : this->objects)
        {
            if (i->intersect(r,h,tmin))
            {
                ifintersect=true;
            }
        }
        return ifintersect;
    }

    void addObject(int index, Object3D *obj) {
        objects.insert(objects.begin()+index,obj);
    }

    int getGroupSize() {
        return objects.size();
    }

private:
    vector<Object3D *> objects;
};

#endif
	

