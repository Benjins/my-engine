#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "../ext/3dbasics.h"

struct SC_Transform;
struct Collider;

struct RBState{
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float invMass;

	float t;
};

struct RBDeriv{
	Vector3 instantVelocity;
	Vector3 instantAcceleration;
};

struct RigidBody{
	float mass;
	bool useGravity;
	float time;

	SC_Transform* transform;

	Collider* col;

	RBState state;
	RBDeriv deriv;

	RigidBody(SC_Transform* _transform, Collider* _col);

	void AddForce(Vector3 force);

	void StepForward(float deltaTime);

	~RigidBody();

};

RBDeriv Evaluate(const RBState& init, float dt, const RBDeriv& d);





#endif