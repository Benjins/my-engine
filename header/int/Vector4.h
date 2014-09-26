#ifndef VECTOR4_H
#define VECTOR4_H

struct Vector3;

struct Vector4{
	float w;
	float x;
	float y;
	float z;

	Vector4();
	Vector4(float _w, float _x, float _y, float _z);
	Vector4(const Vector3& vec, float _w = 1);
	Vector4(float* arrayInit);
};

#endif