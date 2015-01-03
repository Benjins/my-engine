#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "GameObject.h"
#include "Transform.h"
#include "Drawcall.h"
#include "Input.h"
#include <time.h>
#include <random>

using std::list;

struct RigidBody;
struct PhysicsSim;

struct Scene{
	SC_Transform camera;
	float deltaTime;

	PhysicsSim* physicsSim;
	Input input;
	
protected:
	list<GameObject*> objects;
	list<DrawCall> drawCalls;
	int prevX;
	int prevY;
	int buttonDown;
	float xRot;
	float yRot;

	bool running;

	RigidBody* rb;

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

	virtual void Init();

	GameObject* AddObject(GameObject* obj);
	
	void Start();

	void UpdateVertexBuffer();
	
	void OnUpdate();

	void Render();
	void PhysicsUpdate();

	void OnMouse(int button, int state, int x, int y);
	void OnPassiveMouse(int x, int y);
	void OnKey(unsigned char key, int x, int y);
	void OnKeyUp(unsigned char key, int x, int y);

	void Stop();

	virtual ~Scene();
};

static std::default_random_engine myRandom;

static void RenderScene();

static void OnMouseFunc(int button, int state, int x, int y);

static void OnPassiveMouseFunc(int x, int y);

static void OnKeyFunc(unsigned char key, int x, int y);

Mat4x4 GetPerspectiveMatrix(float aspectRatio, float fieldOfView, float nearZ, float farZ);

static void OnKeyUpFunc(unsigned char key, int x, int y);

#endif
