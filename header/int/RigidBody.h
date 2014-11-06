#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "../ext/3dbasics.h"

struct SC_Transform;

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

	RBState state;
	RBDeriv deriv;

	void AddForce(Vector3 force);

	void StepForward(float deltaTime);

};

RBDeriv Evaluate(const RBState& init, float dt, const RBDeriv& d);





#endif