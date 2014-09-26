#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "GameObject.h"
#include "Transform.h"
#include "../header/int/Drawcall.h"

using std::list;

struct Scene{
	list<GameObject> objects;
	list<DrawCall> drawCalls;
	//GameObject Camera;
	
	//NEVER call this version before the parameterized version
	static Scene& getInstance(){
		return getInstance(-1,NULL);
	}
	static Scene& getInstance(int argc, char** argv){
		static Scene instance = Scene(argc, argv);
		return instance;
	}

	Scene();
	Scene(int argc, char** argv);

	GameObject& AddObject(GameObject& obj);
	
	void UpdateVertexBuffer();
	//OnRender
	
	//Camera Transform
	
	void OnUpdate();

	void Render();

	~Scene();
};

static void RenderScene();

#endif