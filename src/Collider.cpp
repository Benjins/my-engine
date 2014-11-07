#include "../header/int/PhysicsSim.h"
#include "../header/int/Collider.h"


Collision BoxCollider::CollisionWith(const Collider* col) const{
	return col->CollisionWith(this);
}
Collision BoxCollider::CollisionWith(const BoxCollider* col) const{
	return DetectCollision(this, col);
}
Collision BoxCollider::CollisionWith(const SphereCollider* col) const{
	return DetectCollision(col, this);
}

void BoxCollider::AddToSim(PhysicsSim* sim){
	sim->staticBoxBodies.push_back(this);
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


Collision DetectCollision(const SphereCollider* col1, const SphereCollider* col2){
	float maxDistance = (col1->radius + col2->radius);
	float maxDistanceSqr = maxDistance*maxDistance;

	float distanceSqr = (col1->position - col2->position).MagnitudeSquared();

	Collision collision;
	collision.collide = distanceSqr < maxDistanceSqr;

	return collision;
}

Collision DetectCollision(const SphereCollider* col1, const BoxCollider* col2){
	//Assumptions: No local transforms, and collider's size has no components less than 0.
	//Also, it's really complicated, and not terribly efficient.  Clean up will occur at not 1:30 AM.

	float radiusSqr = col1->radius * col1->radius;

	bool xBoxInter = (col2->position.x - col2->size.x) < col1->position.x  && col1->position.x < (col2->position.x + col2->size.x);
	bool yBoxInter = (col2->position.y - col2->size.y) < col1->position.y  && col1->position.y < (col2->position.y + col2->size.y);
	bool zBoxInter = (col2->position.z - col2->size.z) < col1->position.z  && col1->position.z < (col2->position.z + col2->size.z);

	bool xExInter = (col2->position.x - col2->size.x - col1->radius) < col1->position.x  && col1->position.x < (col2->position.x + col2->size.x + col1->radius);
	bool yExInter = (col2->position.y - col2->size.y - col1->radius) < col1->position.y  && col1->position.y < (col2->position.y + col2->size.y + col1->radius);
	bool zExInter = (col2->position.z - col2->size.z - col1->radius) < col1->position.z  && col1->position.z < (col2->position.z + col2->size.z + col1->radius);

	bool withinExtendedX = xExInter && yBoxInter && zBoxInter;
	bool withinExtendedY = yBoxInter && yExInter && zBoxInter;
	bool withinExtendedZ = xBoxInter && yBoxInter && zExInter;

	if(withinExtendedX || withinExtendedY || withinExtendedZ){
		Collision col;
		col.collide = true;
		return col;
	}

	float xEdgeDist = min<float>(abs(col2->position.x - col2->size.x - col1->position.x), abs(col2->position.x + col2->size.x - col1->position.x));
	float yEdgeDist = min<float>(abs(col2->position.y - col2->size.y - col1->position.y), abs(col2->position.y + col2->size.y - col1->position.y));
	float zEdgeDist = min<float>(abs(col2->position.z - col2->size.z - col1->position.z), abs(col2->position.z + col2->size.z - col1->position.z));

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

bool RangeCheck(float min, float mid, float max){
	return min < mid && mid < max;
}

//Don't use these; pure virtual declaration caused errors in MSVC.
Collision Collider::CollisionWith(const Collider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const BoxCollider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const SphereCollider* col) const{Collision x; return x;}
void Collider::AddToSim(PhysicsSim* sim){}