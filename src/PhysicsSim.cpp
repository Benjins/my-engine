#include "../header/int/PhysicsSim.h"
#include "../header/int/Collider.h"
#include "../header/int/RigidBody.h"
#include "../header/int/GameObject.h"
#include "../header/int/Transform.h"
#include "../header/int/Interval.h"
#include "../header/int/RaycastHit.h"
#include <string>

PhysicsSim::PhysicsSim(float _deltaTime){
	timeStep = _deltaTime;
	accumulatedTime = 0.0f;
}

void PhysicsSim::AddStaticCollider(Collider* col){
	col->AddToSim(this);
}

void PhysicsSim::AddRigidBody(RigidBody* rb){
	dynamicBodies.push_back(rb);
}

void PhysicsSim::Advance(float dt){
	accumulatedTime += dt;

	while(accumulatedTime >= timeStep){
		StepForward();
		accumulatedTime -= timeStep;
	}
}

void PhysicsSim::StepForward(){
	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		(*iter)->StepForward(timeStep);
		for(auto iter2 = staticBoxBodies.begin(); iter2 != staticBoxBodies.end(); iter2++){

			if((*iter)->col->CollisionWith(*iter2).collide){
				GameObject* obj1 = (*iter)->col->gameObject;
				GameObject* obj2 = (*iter2)->gameObject;

				obj1->OnCollision(*iter2);
				obj2->OnCollision((*iter)->col);
			}
		}
	}
}

RaycastHit PhysicsSim::Raycast(Vector3 origin, Vector3 direction){
	for(auto iter = staticBoxBodies.begin(); iter != staticBoxBodies.end(); iter++){
		RaycastHit hit = RaycastBox(*iter, origin, direction);
		if(hit.hit){
			return hit;
		}
	}

	RaycastHit x;
	x.hit = false;
	return x;
}

RaycastHit RaycastBox(BoxCollider* col, Vector3 origin, Vector3 direction){
	Vector3 colMin = col->position - col->size;
	Vector3 colMax = col->position + col->size;

	SC_Transform trans= col->gameObject->transform;
	Vector3 transformedOrigin = trans.GlobalToLocal(origin);
	Vector3 transformedDirection = Rotate(direction, trans.rotation);

	Vector3 corner1 = origin - colMin;
	Vector3 corner2 = origin - colMax;

	Vector3 diffMin = Vector3(  min(corner1.x, corner2.x),
								min(corner1.y, corner2.y),
								min(corner1.z, corner2.z));
	Vector3 diffMax = Vector3(  max(corner1.x, corner2.x),
								max(corner1.y, corner2.y),
								max(corner1.z, corner2.z));

	Interval xInter = Interval(diffMin.x / direction.x, diffMax.x / direction.x, true);
	Interval yInter = Interval(diffMin.y / direction.y, diffMax.y / direction.y, true);
	Interval zInter = Interval(diffMin.z / direction.z, diffMax.z / direction.z, true);

	Interval yzInter = yInter.Intersection(zInter);

	Interval lineInterval = xInter.Intersection(yzInter);

	if(lineInterval.IsValid() && lineInterval.min > 0){
		RaycastHit x;
		x.hit = true;
		x.col = col;
		return x;
	}

	RaycastHit x;
	x.hit = false;
	return x;
}

PhysicsSim::~PhysicsSim(){
	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		delete (*iter);
	}

	//We don't actually destroy the static colliders, since they're components, 
	//and so the gameobject they're attached to will handle them.
}

