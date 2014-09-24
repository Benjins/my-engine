#ifndef COMPONENT_H
#define COMPONENT_H

struct GameObject;

struct Component{
  protected: 
	const static bool isComponent = true;
  public:
	GameObject* gameObject;
	
	Component();
	
	virtual void OnAwake();
	
	virtual void OnUpdate();
	
	virtual ~Component();
};

#endif