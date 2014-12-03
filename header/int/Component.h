#ifndef COMPONENT_H
#define COMPONENT_H

struct GameObject;
struct Collider;

struct Component{
	const static bool isComponent = true;
	GameObject* gameObject;
	
	Component();
	
	virtual void OnAwake();
	
	virtual void OnUpdate();

	virtual void OnCollision(Collider* col);
	
	virtual ~Component();
};

#endif