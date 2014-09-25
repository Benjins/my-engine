#ifndef COMPONENT_H
#define COMPONENT_H

struct GameObject;

struct Component{
	const static bool isComponent = true;
	GameObject* gameObject;
	
	Component();
	
	virtual void OnAwake();
	
	virtual void OnUpdate();
	
	virtual ~Component();
};

#endif