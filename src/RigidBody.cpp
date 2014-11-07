#include "../header/int/RigidBody.h"
#include "../header/int/Transform.h"
#include "../header/int/Collider.h"
#include "../header/int/GameObject.h"
#include "../header/int/Scene.h"
#include "../header/int/PhysicsSim.h"


RigidBody::RigidBody(SC_Transform* _transform, Collider* _col){
	transform = _transform;
	col = _col;
	//cout << "transform->gameObject->scene: " << (long)transform->gameObject->scene << endl;
	transform->gameObject->scene->physicsSim->AddRigidBody(this);
	state.position = transform->position;
}

void RigidBody::AddForce(Vector3 force){

	state.force = state.force + force;
}

void RigidBody::StepForward(float deltaTime){
	RBDeriv a,b,c,d,initial;

	initial.instantAcceleration = initial.instantVelocity = Vector3(0,0,0);

	a = Evaluate(state, 0, initial);
	b = Evaluate(state, deltaTime/2, a);
	c = Evaluate(state, deltaTime/2, b);
	d = Evaluate(state, deltaTime,   c);

	Vector3 dxdt = (a.instantVelocity + b.instantVelocity * 2 
				  + c.instantVelocity * 2 + d.instantVelocity) * 1.0f/6;
	Vector3 dvdt = (a.instantAcceleration + b.instantAcceleration * 2 
				  + c.instantAcceleration * 2 + d.instantAcceleration) * 1.0f/6;

	deriv.instantVelocity = dxdt;
	deriv.instantAcceleration = dvdt;

	state.position = state.position + dxdt * deltaTime;
	transform->position = state.position;
	state.velocity = state.velocity + dvdt * deltaTime;
}

RBDeriv Evaluate(const RBState& init, float dt, const RBDeriv& d){

	RBState state;
	state.position = init.position + d.instantVelocity * dt;
	state.velocity = init.velocity + d.instantAcceleration * dt;

	RBDeriv deriv;
	deriv.instantVelocity = state.velocity;
	deriv.instantAcceleration = state.position * -10 - state.velocity;
	//For now, acceleration is hardcoded as a spring force

	return deriv;
}

RigidBody::~RigidBody(){
	delete col;
}