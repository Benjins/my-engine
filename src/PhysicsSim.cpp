#include "../header/int/PhysicsSim.h"
#include "../header/int/Collider.h"
#include "../header/int/RigidBody.h"
#include "../header/int/GameObject.h"
#include "../header/int/Transform.h"
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

PhysicsSim::~PhysicsSim(){
	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		delete (*iter);
	}

	//We don't actually destroy the static colliders, since they're components, 
	//and so the gameobject they're attached to will handle them.
}

