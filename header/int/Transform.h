#ifndef SC_TRANSFORM_H
#define SC_TRANSFORM_H

#include "../ext/3dbasics.h"

struct GameObject;
struct Mat4x4;

struct SC_Transform{
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
	
	GameObject* gameObject;
	SC_Transform* parent;
	
	SC_Transform();
    SC_Transform(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, SC_Transform* _parent = NULL);
	
	Vector3 Forward() const;
	Vector3 Up() const;
	Vector3 Right() const;
	
	Mat4x4 LocalToGlobalMatrix() const;

	SC_Transform GetInverse() const;

	Vector3 GlobalPosition() const;
	
    //Transform a vector into this transform's space
    Vector3 GlobalToLocal(const Vector3& original) const;

    //Transform a vector from this transfom's space to global
	Vector3 LocalToGlobal(const Vector3& original) const;
};

#endif