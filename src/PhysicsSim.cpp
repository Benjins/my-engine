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
		RigidBody* rb = *iter;
		if(rb->isKinematic){
			rb->StepForward(timeStep);
		}
		for(auto iter2 = dynamicBodies.begin(); iter2 != dynamicBodies.end(); iter2++){
			RigidBody* rb2 = *iter2;
			if(rb == rb2){
				continue;
			}

			Collision collision = rb->col->CollisionWith(rb2->col);
			if(collision.collide){
				GameObject* obj1 = rb->col->gameObject;
				GameObject* obj2 = rb2->col->gameObject;

				obj1->OnCollision(rb2->col);
				obj2->OnCollision(rb->col);
			}
		}
		for(auto iter2 = staticBoxBodies.begin(); iter2 != staticBoxBodies.end(); iter2++){
			if((*iter)->col == *iter2){
				continue;
			}

			if((*iter2)->gameObject->name == "floor" && (*iter)->gameObject->name != "mainCam"){
				int x = 0;
			}

			Collision collision = (*iter)->col->CollisionWith(*iter2);
			if(collision.collide){
				GameObject* obj1 = rb->col->gameObject;
				GameObject* obj2 = (*iter2)->gameObject;

				obj1->OnCollision(*iter2);
				obj2->OnCollision(rb->col);
			}
		}
	}
}

RaycastHit PhysicsSim::Raycast(Vector3 origin, Vector3 direction){
	direction.Normalize();
	RaycastHit finalHit;
	finalHit.hit = false;
	finalHit.depth = FLT_MAX;

	for(auto iter = staticBoxBodies.begin(); iter != staticBoxBodies.end(); iter++){
		RaycastHit hit = RaycastBox(*iter, origin, direction);
		if(hit.hit && hit.depth < finalHit.depth){
			finalHit = hit;
		}
	}

	for(auto iter = staticSphereBodies.begin(); iter != staticSphereBodies.end(); iter++){
		RaycastHit hit = RaycastSphere(*iter, origin, direction);
		if(hit.hit && hit.depth < finalHit.depth){
			finalHit = hit;
		}
	}

	return finalHit;
}

RaycastHit RaycastSphere(SphereCollider* col, Vector3 origin, Vector3 direction){
	SC_Transform trans = col->gameObject->transform;
	Vector3 transformedColPosition = trans.LocalToGlobal(col->position);
	float transformedColRadius = trans.TotalScale().x * col->radius;

	Vector3 originToCentre = transformedColPosition - origin;
	Vector3 projection = VectorProject(originToCentre, direction);
	float distance = (origin + projection - transformedColPosition).Magnitude();

	if(distance > transformedColRadius){
		RaycastHit hit;
		hit.hit = false;
		return hit;
	}

	RaycastHit hit;
	hit.hit = true;
	hit.depth = 0;
	hit.col = col;
	return hit;
}

RaycastHit RaycastBox(BoxCollider* col, Vector3 origin, Vector3 direction){
	Vector3 colMin = col->position - col->size;
	Vector3 colMax = col->position + col->size;

	SC_Transform trans = col->gameObject->transform;
	Vector3 transformedOrigin = trans.GlobalToLocal(origin);
	Vector3 transformedDirection = trans.GlobalToLocal(direction) - trans.GlobalToLocal(Vector3(0,0,0));

	Vector3 gameObjectScale = col->gameObject->transform.scale;
	Vector3 corner1 = colMin - transformedOrigin;
	Vector3 corner2 = colMax - transformedOrigin;

	Vector3 diffMin = Vector3(min(corner1.x, corner2.x),
							  min(corner1.y, corner2.y),
							  min(corner1.z, corner2.z));
	Vector3 diffMax = Vector3(max(corner1.x, corner2.x),
							  max(corner1.y, corner2.y),
							  max(corner1.z, corner2.z));

	Interval xInter = Interval(diffMin.x / transformedDirection.x, diffMax.x / transformedDirection.x, true);
	Interval yInter = Interval(diffMin.y / transformedDirection.y, diffMax.y / transformedDirection.y, true);
	
	Interval zInter = Interval(diffMin.z / transformedDirection.z, diffMax.z / transformedDirection.z, true);

	Interval yzInter = yInter.Intersection(zInter);

	Interval lineInterval = xInter.Intersection(yzInter);

	if(lineInterval.IsValid() && lineInterval.min > 0){
		RaycastHit x;
		x.depth = lineInterval.min;
		x.worldPos = origin + direction*x.depth;
		x.hit = true;
		x.col = col;

		Vector3 transformedHit = (transformedOrigin + transformedDirection * x.depth) - col->position;
		transformedHit = transformedHit.Scaled(Vector3(1 / col->size.x, 1 / col->size.y, 1 / col->size.z));

		Vector3 normalLocal = transformedHit;
		normalLocal.x = (abs(abs(normalLocal.x) - 1) < 0.0000001f ? 1 : 0) * (normalLocal.x < 0 ? -1 : 1);
		normalLocal.y = (abs(abs(normalLocal.y) - 1) < 0.0000001f ? 1 : 0) * (normalLocal.y < 0 ? -1 : 1);
		normalLocal.z = (abs(abs(normalLocal.z) - 1) < 0.0000001f ? 1 : 0) * (normalLocal.z < 0 ? -1 : 1);

		x.normal = Rotate(normalLocal, trans.TotalRotation()).Normalized();
		return x;
	}

	RaycastHit x;
	x.hit = false;
	return x;
}

PhysicsSim::~PhysicsSim(){
	//We don't actually destroy the static colliders or rigidbodies, since they're components, 
	//and so the gameobject they're attached to will handle them.
}

