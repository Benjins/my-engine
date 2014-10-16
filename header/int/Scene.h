#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "GameObject.h"
#include "Transform.h"
#include "Drawcall.h"
#include <time.h>

using std::list;

struct Scene{
	list<GameObject*> objects;
	list<DrawCall> drawCalls;
	SC_Transform camera;
	float deltaTime;

	int prevX;
	int prevY;
	int buttonDown;

private:
	clock_t prevTime;
public:
	
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

	GameObject* AddObject(GameObject* obj);
	
	void UpdateVertexBuffer();
	
	void OnUpdate();

	void Render();

	void OnMouse(int button, int state, int x, int y);
	void OnPassiveMouse(int x, int y);
	void OnKey(unsigned char key, int x, int y);

	~Scene();
};

static void RenderScene();

static void OnMouseFunc(int button, int state, int x, int y);

static void OnPassiveMouseFunc(int x, int y);

static void OnKeyFunc(unsigned char key, int x, int y);

#endif
