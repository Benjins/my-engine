#ifndef PHYSICS_SIM_H
#define PHYSICS_SIM_H

#include <list>

using std::list;

struct Collider; struct BoxCollider; struct SphereCollider; struct RigidBody;

struct PhysicsSim{
	float timeStep;
	float accumulatedTime;

	PhysicsSim(float _deltaTime = 0.02f);

	list<RigidBody*> dynamicBodies;
	list<BoxCollider*> staticBoxBodies;
	list<SphereCollider*> staticSphereBodies;

	void AddStaticCollider(Collider* col);
	void AddRigidBody(RigidBody* rb);
	
	void Advance(float dt);

protected:
	void StepForward();

public:

	~PhysicsSim();
};

#endif