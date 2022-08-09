#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {
	

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0]=a;
		vertices[1]=b;
		vertices[2]=c;

	}

	bool intersect( const Ray& ray,  Hit& hit , double tmin) override {
		Vector3f R0=ray.getOrigin();
		Vector3f Rd=ray.getDirection();
		Vector3f E1=vertices[0]-vertices[1];
		Vector3f E2=vertices[0]-vertices[2];
		Vector3f S=vertices[0]-R0;
		Matrix3f matrix1(Rd,E1,E2);
		Matrix3f matrix2(S,E1,E2);
		Matrix3f matrix3(Rd,S,E2);
		Matrix3f matrix4(Rd,E1,S);
		Vector3f vec1(matrix2.determinant(),matrix3.determinant(),matrix4.determinant());
		Vector3f vec2=(1 / matrix1.determinant()) * vec1;
		double t=vec2[0],beta=vec2[1],gama=vec2[2];
		Vector3f n= Vector3f::cross(E1,E2);
		if( t>0 && beta>=0 && gama>=0 && (beta+gama)<=1)
		{
			if( t> tmin && t<hit.getT())
			{
				hit.set(t,Object3D::material,n.normalized());
				return true;
			}
		}
        return false;
	}
	Vector3f normal;
	Vector3f vertices[3];
protected:

};

#endif //TRIANGLE_H
