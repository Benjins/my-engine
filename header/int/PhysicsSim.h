#ifndef PHYSICS_SIM_H
#define PHYSICS_SIM_H

#include <vector>

using std::vector;

struct Collider; struct BoxCollider; struct SphereCollider; struct RigidBody;

struct PhysicsSim{
	float timeStep;
	float accumulatedTime;

	PhysicsSim(float _deltaTime = 0.02f);

	vector<RigidBody*> dynamicBodies;
	vector<BoxCollider*> staticBoxBodies;
	vector<SphereCollider*> staticSphereBodies;

	void AddStaticCollider(Collider* col);
	void AddRigidBody(RigidBody* rb);
	
	void Advance(float dt);

protected:
	void StepForward();

public:

	~PhysicsSim();
};

#endif