#ifndef RAYCAST_HIT_H
#define RAYCAST_HIT_H

#include "../ext/Vector3.h"
#include "../ext/Vector2.h"

struct Collider;

struct RaycastHit{
	Vector3 worldPos;
	bool hit;
	Vector2 texCoords;

	Collider* col;

	RaycastHit();
};


#endif