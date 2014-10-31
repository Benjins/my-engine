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

Collision SphereCollider::CollisionWith(const Collider* col) const{
	return col->CollisionWith(this);
}
Collision SphereCollider::CollisionWith(const BoxCollider* col) const{
	return DetectCollision(this, col);
}
Collision SphereCollider::CollisionWith(const SphereCollider* col) const{
	return DetectCollision(this, col);
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

	float xEdgeDist = min<float>(abs(col2->position.x - col2->size.x - col1->position.x), abs(col2->position.x + col2->size.x - col1->position.x));
	float yEdgeDist = min<float>(abs(col2->position.y - col2->size.y - col1->position.y), abs(col2->position.y + col2->size.y - col1->position.y));
	float zEdgeDist = min<float>(abs(col2->position.z - col2->size.z - col1->position.z), abs(col2->position.z + col2->size.z - col1->position.z));

	float xEdgeDistSqr = xEdgeDist*xEdgeDist;
	float yEdgeDistSqr = yEdgeDist*yEdgeDist;
	float zEdgeDistSqr = zEdgeDist*zEdgeDist;

	bool withinEdgeXY = xExInter && yExInter && zBoxInter && (xEdgeDistSqr + yEdgeDistSqr < radiusSqr);
	bool withinEdgeXZ = xExInter && yBoxInter && zExInter && (xEdgeDistSqr + zEdgeDistSqr < radiusSqr);
	bool withinEdgeYZ = xBoxInter && yExInter && zExInter && (yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	bool withinCorner = xExInter && yExInter && zExInter && (xEdgeDistSqr + yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	bool result = withinExtendedX || withinExtendedY || withinExtendedZ || withinEdgeXY || withinEdgeXZ || withinEdgeYZ || withinCorner;

	Collision col;
	col.collide = result;
	return col;
}

Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2){
	Collision x;
	return x;
}

//Don't use these; pure virtual declaration caused errors in MSVC.
Collision Collider::CollisionWith(const Collider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const BoxCollider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const SphereCollider* col) const{Collision x; return x;}