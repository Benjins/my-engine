#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component.h"
#include  "../ext/Vector3.h"

struct BoxCollider;
struct SphereCollider;

struct Collision{
	bool collide;
	Vector3 normal;
	float depth;
};

struct Collider : Component{
	GameObject* gameObject;

	virtual Collision CollisionWith(const Collider* col) const;
	virtual Collision CollisionWith(const BoxCollider* col) const;
	virtual Collision CollisionWith(const SphereCollider* col) const;
};

struct BoxCollider : Collider{
	Vector3 position;
	Vector3 size;
	virtual Collision CollisionWith(const Collider* col) const;
	virtual Collision CollisionWith(const BoxCollider* col) const;
	virtual Collision CollisionWith(const SphereCollider* col) const;
};

struct SphereCollider : Collider{
	Vector3 position;
	float radius;
	virtual Collision CollisionWith(const Collider* col) const;
	virtual Collision CollisionWith(const BoxCollider* col) const;
	virtual Collision CollisionWith(const SphereCollider* col) const;
};

Collision DetectCollision(const SphereCollider* col1, const SphereCollider* col2);
Collision DetectCollision(const SphereCollider* col1, const BoxCollider* col2);
Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2);

#endif