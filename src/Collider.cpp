#include "../header/int/PhysicsSim.h"
#include "../header/int/GameObject.h"
#include "../header/int/Transform.h"
#include "../header/int/Mat4.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"
#include "../header/int/AABB.h"


Collision BoxCollider::CollisionWith(const Collider* col) const{
	return col->CollisionWith(this);
}
Collision BoxCollider::CollisionWith(const BoxCollider* col) const{
	return DetectCollision(this, col);
}
Collision BoxCollider::CollisionWith(const SphereCollider* col) const{
	return DetectCollision(col, this);
}

Collider::Collider(){
	gameObject = NULL;
}

BoxCollider::BoxCollider(Vector3 _position, Vector3 _size){
	gameObject = NULL;
	position = _position;
	size = _size;
}

AABB BoxCollider::GetBounds(bool globalSpace) const{
	return AABB(this, globalSpace);
}

void BoxCollider::AddToSim(PhysicsSim* sim){
	sim->staticBoxBodies.push_back(this);
}

void BoxCollider::OnAwake(){
	if(gameObject != NULL && gameObject->scene != NULL){
		AddToSim(gameObject->scene->physicsSim);
	}
}

BoxCollider::~BoxCollider(){
}

SphereCollider::SphereCollider(Vector3 _position, float _radius){
	gameObject = NULL;
	position = _position;
	radius = _radius;
}

Collision SphereCollider::CollisionWith(const Collider* col) const{
	return col->CollisionWith(this);
}
Collision SphereCollider::CollisionWith(const BoxCollider* col) const{
	return DetectCollision(this, col);
}
Collision SphereCollider::CollisionWith(const SphereCollider* col) const{
	return DetectCollision(this, col);
}

void SphereCollider::AddToSim(PhysicsSim* sim){
	sim->staticSphereBodies.push_back(this);
}

void SphereCollider::OnAwake(){
	if(gameObject->scene != NULL){
		AddToSim(gameObject->scene->physicsSim);
	}
}

SphereCollider::~SphereCollider(){

}

Collision DetectCollision(const SphereCollider* col1, const SphereCollider* col2){
	float maxDistance = (col1->radius + col2->radius);
	float maxDistanceSqr = maxDistance*maxDistance;

	Vector3 col1TransformedPos = col1->position;
	if(col2->gameObject != NULL && col2->gameObject != NULL){
		col1TransformedPos = col2->gameObject->transform.GlobalToLocal(col1->gameObject->transform.LocalToGlobal(col1->position));
	}
	float distanceSqr = (col1TransformedPos - col2->position).MagnitudeSquared();

	Collision collision;
	collision.collide = distanceSqr < maxDistanceSqr;

	return collision;
}

Collision DetectCollision(const SphereCollider* col1, const BoxCollider* col2){
	//Assumptions: No local transforms, and collider's size has no components less than 0.
	//Also, it's really complicated, and not terribly efficient.  Clean up will occur at not 1:30 AM.

	float radiusSqr = col1->radius * col1->radius;

	Vector3 spherePos = col1->position;

	if(col1->gameObject != NULL && col2->gameObject != NULL){
		spherePos = col2->gameObject->transform.GlobalToLocal(col1->gameObject->transform.LocalToGlobal(col1->position));
	}

	
	bool xBoxInter = RangeCheck(col2->position.x - col2->size.x, spherePos.x, col2->position.x + col2->size.x);
	bool yBoxInter = RangeCheck(col2->position.y - col2->size.y, spherePos.y, col2->position.y + col2->size.y);
	bool zBoxInter = RangeCheck(col2->position.z - col2->size.z, spherePos.z, col2->position.z + col2->size.z);

	
	bool xExInter = RangeCheck(col2->position.x - col2->size.x - col1->radius, spherePos.x, col2->position.x + col2->size.x + col1->radius);
	bool yExInter = RangeCheck(col2->position.y - col2->size.y - col1->radius, spherePos.y, col2->position.y + col2->size.y + col1->radius);
	bool zExInter = RangeCheck(col2->position.z - col2->size.z - col1->radius, spherePos.z, col2->position.z + col2->size.z + col1->radius);

	bool withinExtendedX = xExInter && yBoxInter && zBoxInter;
	bool withinExtendedY = xBoxInter && yExInter && zBoxInter;
	bool withinExtendedZ = xBoxInter && yBoxInter && zExInter;

	if(withinExtendedX || withinExtendedY || withinExtendedZ){
		Collision col;
		col.collide = true;
		return col;
	}

	float xEdgeDist = min<float>(abs(col2->position.x - col2->size.x - spherePos.x), abs(col2->position.x + col2->size.x - spherePos.x));
	float yEdgeDist = min<float>(abs(col2->position.y - col2->size.y - spherePos.y), abs(col2->position.y + col2->size.y - spherePos.y));
	float zEdgeDist = min<float>(abs(col2->position.z - col2->size.z - spherePos.z), abs(col2->position.z + col2->size.z - spherePos.z));

	float xEdgeDistSqr = xEdgeDist*xEdgeDist;
	float yEdgeDistSqr = yEdgeDist*yEdgeDist;
	float zEdgeDistSqr = zEdgeDist*zEdgeDist;

	bool withinEdgeXY = xExInter && yExInter && zBoxInter && (xEdgeDistSqr + yEdgeDistSqr < radiusSqr);
	bool withinEdgeXZ = xExInter && yBoxInter && zExInter && (xEdgeDistSqr + zEdgeDistSqr < radiusSqr);
	bool withinEdgeYZ = xBoxInter && yExInter && zExInter && (yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	if(withinEdgeXY || withinEdgeXZ || withinEdgeYZ){
		Collision col;
		col.collide = true;
		return col;
	}

	bool withinCorner = xExInter && yExInter && zExInter && (xEdgeDistSqr + yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	if(withinCorner){
		Collision col;
		col.collide = true;
		return col;
	}
	
	Collision col;
	col.collide = false;

	return col;
}

Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2){
	AABB col1TransBounds = AABB(col1->GetBounds(),col2->gameObject->transform.GlobalToLocalMatrix());
	AABB col2TransBounds = AABB(col2->GetBounds(),col1->gameObject->transform.GlobalToLocalMatrix());

	AABB col1Bounds = col1->GetBounds(false);
	AABB col2Bounds = col2->GetBounds(false);

	if(col1Bounds.CollideWith(col2TransBounds).collide){
		Collision x;
		x.collide = true;
		return x;
	}

	if(col2Bounds.CollideWith(col1TransBounds).collide){
		Collision x;
		x.collide = true;
		return x;
	}

	Collision x;
	x.collide = false;
	return x;
}

/*
Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2){
	Vector3 maxOne = col1->position + col1->size;
	Vector3 minOne = col1->position - col1->size;
	Vector3 transMaxOne = maxOne;
	Vector3 transMinOne = minOne;
	if(col1->gameObject != NULL && col2->gameObject != NULL){
		transMaxOne = col2->gameObject->transform.GlobalToLocal(col1->gameObject->transform.LocalToGlobal(maxOne));
		transMinOne = col2->gameObject->transform.GlobalToLocal(col1->gameObject->transform.LocalToGlobal(minOne));
	}

	Vector3 maxTwo = col2->position + col2->size;
	Vector3 minTwo = col2->position - col2->size;
	Vector3 transMaxTwo = maxOne;
	Vector3 transMinTwo = minOne;
	if(col1->gameObject != NULL && col2->gameObject != NULL){
		transMaxTwo = col1->gameObject->transform.GlobalToLocal(col2->gameObject->transform.LocalToGlobal(maxTwo));
		transMinTwo = col1->gameObject->transform.GlobalToLocal(col2->gameObject->transform.LocalToGlobal(minTwo));
	}

	bool xIntersection =   RangeCheck(minOne.x, transMinTwo.x, maxOne.x) || RangeCheck(minOne.x, transMaxTwo.x, maxOne.x)
						|| RangeCheck(minTwo.x, transMinOne.x, maxTwo.x) || RangeCheck(minTwo.x, transMaxOne.x, maxTwo.x);

	bool yIntersection =   RangeCheck(minOne.y, transMinTwo.y, maxOne.y) || RangeCheck(minOne.y, transMaxTwo.y, maxOne.y)
						|| RangeCheck(minTwo.y, transMinOne.y, maxTwo.y) || RangeCheck(minTwo.y, transMaxOne.y, maxTwo.y);

	bool zIntersection =   RangeCheck(minOne.z, transMinTwo.z, maxOne.z) || RangeCheck(minOne.z, transMaxTwo.z, maxOne.z)
						|| RangeCheck(minTwo.z, transMinOne.z, maxTwo.z) || RangeCheck(minTwo.z, transMaxOne.z, maxTwo.z);

	Collision x;
	x.collide = xIntersection && yIntersection && zIntersection;
	return x;
}
*/

Collision DetectCollision(const BoxCollider* col1, const SphereCollider* col2){
	return DetectCollision(col2, col1);
}

//Check that mid is less than min and greater than max
bool RangeCheck(float min, float mid, float max){
	return min <= mid && mid <= max;
}

//Don't use these; pure virtual declaration caused errors in MSVC.
Collision Collider::CollisionWith(const Collider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const BoxCollider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const SphereCollider* col) const{Collision x; return x;}
void Collider::AddToSim(PhysicsSim* sim){}
void Collider::OnAwake(){}
Collider::~Collider(){

}