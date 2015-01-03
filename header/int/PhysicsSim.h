#ifndef PHYSICS_SIM_H
#define PHYSICS_SIM_H

#include <vector>

using std::vector;

struct Collider; struct BoxCollider; struct SphereCollider; struct RigidBody; struct RaycastHit; struct Vector3;

struct PhysicsSim{
	float timeStep;
	float accumulatedTime;

	vector<RigidBody*> dynamicBodies;
	vector<BoxCollider*> staticBoxBodies;
	vector<SphereCollider*> staticSphereBodies;

	PhysicsSim(float _deltaTime = 0.02f);

	void AddStaticCollider(Collider* col);
	void AddRigidBody(RigidBody* rb);

	RaycastHit Raycast(Vector3 origin, Vector3 direction);
	
	void Advance(float dt);

protected:
	void StepForward();

public:

	~PhysicsSim();
};

RaycastHit RaycastBox(BoxCollider* col, Vector3 origin, Vector3 direction);

#endif