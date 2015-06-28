#ifndef COMPONENT_H
#define COMPONENT_H

struct GameObject;
struct Collider;
struct XMLElement;

struct Component{
	const static bool isComponent = true;
	GameObject* gameObject;
	
	Component();
	
	virtual void OnAwake();
	
	virtual void OnUpdate();

	virtual void OnEditorUpdate(){}

	virtual Component* Clone(){return new Component();}

	virtual void OnCollision(Collider* col);

	virtual XMLElement Serialize();
	virtual void Deserialize(const XMLElement& data);
	
	virtual ~Component();
};

#endif