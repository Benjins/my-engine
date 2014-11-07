#include "../header/int/PhysicsSim.h"
#include "../header/int/Collider.h"
#include "../header/int/RigidBody.h"

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
	}
}

PhysicsSim::~PhysicsSim(){
	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		delete (*iter);
	}

	for(auto iter = staticBoxBodies.begin(); iter != staticBoxBodies.end(); iter++){
		delete (*iter);
	}

	for(auto iter = staticSphereBodies.begin(); iter != staticSphereBodies.end(); iter++){
		delete (*iter);
	}
}

