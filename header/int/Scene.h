#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "GameObject.h"
#include "Transform.h"

using std::list;

struct Scene{
	list<GameObject> objects;
	//GameObject Camera;
	
	GameObject& AddObject(GameObject& obj);
	
	//Update VertexBuffer
	//OnRender
	
	//Camera Transform
	
	//DrawCall struct, takes Material and Mesh
	//Also define material, mesh, etc. structs
	//tired....
	
	Scene();
	
	void OnUpdate();
};

#endif