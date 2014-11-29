#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component.h"
#include  "../ext/Vector3.h"

struct BoxCollider;
struct SphereCollider;
struct PhysicsSim;
struct AABB;

struct Collision{
	bool collide;
	Vector3 normal;
	float depth;
};

struct Collider : Component{
	Collider();

	virtual Collision CollisionWith(const Collider* col) const;
	virtual Collision CollisionWith(const BoxCollider* col) const;
	virtual Collision CollisionWith(const SphereCollider* col) const;
	virtual void AddToSim(PhysicsSim* sim);
	virtual void OnAwake();

	virtual ~Collider();
};

struct BoxCollider : Collider{
	Vector3 position;
	Vector3 size;

	BoxCollider(Vector3 _position = Vector3(0,0,0), Vector3 _size = Vector3(1,1,1));

	AABB GetBounds(bool globalSpace = true) const;

	virtual Collision CollisionWith(const Collider* col) const;
	virtual Collision CollisionWith(const BoxCollider* col) const;
	virtual Collision CollisionWith(const SphereCollider* col) const;
	virtual void AddToSim(PhysicsSim* sim);
	virtual void OnAwake();

	virtual ~BoxCollider();
};

struct SphereCollider : Collider{
	Vector3 position;
	float radius;

	SphereCollider(Vector3 _position = Vector3(0,0,0), float _radius = 0.5f);

	virtual Collision CollisionWith(const Collider* col) const;
	virtual Collision CollisionWith(const BoxCollider* col) const;
	virtual Collision CollisionWith(const SphereCollider* col) const;
	virtual void AddToSim(PhysicsSim* sim);
	virtual void OnAwake();

	virtual ~SphereCollider();
};

Collision DetectCollision(const SphereCollider* col1, const SphereCollider* col2);
Collision DetectCollision(const SphereCollider* col1, const BoxCollider* col2);
Collision DetectCollision(const BoxCollider* col1, const SphereCollider* col2);
Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2);

bool RangeCheck(float min, float mid, float max);

#endif