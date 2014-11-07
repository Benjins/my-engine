#include "../header/int/PhysicsSim.h"
#include "../header/int/GameObject.h"
#include "../header/int/Transform.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"


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

void BoxCollider::AddToSim(PhysicsSim* sim){
	sim->staticBoxBodies.push_back(this);
}

void BoxCollider::OnAwake(){
	cout << "BoxCollider::OnAwake()" << endl;
	if(gameObject->scene != NULL){
		cout << "AddToSim(gameObject->scene->physicsSim);" << endl;
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
		//cout << "Within: " << (withinExtendedX? " exX " : "") << (withinExtendedY? " exY " : "") << (withinExtendedZ? " exZ " : "") << endl;
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
		cout << "Within EdgeXY, EdgeXZ,or EdgeYZ\n";
		Collision col;
		col.collide = true;
		return col;
	}

	bool withinCorner = xExInter && yExInter && zExInter && (xEdgeDistSqr + yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	if(withinCorner){
		cout << "Within corner\n";
		Collision col;
		col.collide = true;
		return col;
	}
	
	Collision col;
	col.collide = false;

	return col;
}

Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2){
	Vector3 maxOne = col1->position + col1->size;
	Vector3 minOne = col1->position - col1->size;

	Vector3 maxTwo = col2->position + col2->size;
	Vector3 minTwo = col2->position - col2->size;

	bool xIntersection =   RangeCheck(minOne.x, minTwo.x, maxOne.x) || RangeCheck(minOne.x, maxTwo.x, maxOne.x)
						|| RangeCheck(minTwo.x, minOne.x, maxTwo.x) || RangeCheck(minTwo.x, maxOne.x, maxTwo.x);

	bool yIntersection =   RangeCheck(minOne.y, minTwo.y, maxOne.y) || RangeCheck(minOne.y, maxTwo.y, maxOne.y)
						|| RangeCheck(minTwo.y, minOne.y, maxTwo.y) || RangeCheck(minTwo.y, maxOne.y, maxTwo.y);

	bool zIntersection =   RangeCheck(minOne.z, minTwo.z, maxOne.z) || RangeCheck(minOne.z, maxTwo.z, maxOne.z)
						|| RangeCheck(minTwo.z, minOne.z, maxTwo.z) || RangeCheck(minTwo.z, maxOne.z, maxTwo.z);

	

	Collision x;
	x.collide = xIntersection && yIntersection && zIntersection;
	return x;
}

Collision DetectCollision(const BoxCollider* col1, const SphereCollider* col2){
	return DetectCollision(col2, col1);
}

//Check that mid is less than min and greater than max
bool RangeCheck(float min, float mid, float max){
	return min < mid && mid < max;
}

//Don't use these; pure virtual declaration caused errors in MSVC.
Collision Collider::CollisionWith(const Collider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const BoxCollider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const SphereCollider* col) const{Collision x; return x;}
void Collider::AddToSim(PhysicsSim* sim){}
void Collider::OnAwake(){}
Collider::~Collider(){

}