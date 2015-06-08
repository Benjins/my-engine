#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "GameObject.h"
#include "Transform.h"
#include "Drawcall.h"
#include "Input.h"
#include "ResourceManager.h"
#include "GuiElement.h"
#include "Pathfinding.h"
#include "Light.h"
#include <time.h>
#include <random>

using std::list;

struct RigidBody;
struct PhysicsSim;
struct XMLElement;

struct Scene{
	SC_Transform* camera;
	float deltaTime;

	PhysicsSim* physicsSim;
	Input input;
	MaterialManager resources;
	Pathfinding pathfinding;

	//Vector3 lightDir;
	vector<GuiElement*> guiElements;
	vector<Light> lights;
	
protected:
	vector<GameObject*> objects;
	vector<DrawCall> drawCalls;

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
		return getInstance(0,NULL);
	}
	static Scene& getInstance(int argc, char** argv){
		static Scene instance = Scene(argc, argv);
		return instance;
	}

	Scene();
	Scene(int argc, char** argv);

	int AddLight();

protected:
	//Hide this, because it'll break things
	Scene(const Scene& orig){}

public:

	virtual void Init();

	void LoadScene(string fileName);
	void SaveScene(string fileName);

	GameObject* AddObject(GameObject* obj);
	GuiElement* AddGuiElement();

	void RemoveObject(GameObject* obj);
	
	virtual void Start();

	void UpdateVertexBuffer();
	
	void OnUpdate();

	void OnPostLoad();

	void Render();
	void PhysicsUpdate();

	GameObject* FindGameObject(const string& name);

	GuiElement* FindGUIElement(const string& name);

	void OnMouse(int button, int state, int x, int y);
	void OnPassiveMouse(int x, int y);
	void OnKey(unsigned char key, int x, int y);
	void OnKeyUp(unsigned char key, int x, int y);

	void RemoveAllObjects();

	void Stop();

	void LoadGuiElement(const XMLElement& elem);
	void LoadGuiText(const XMLElement& elem);

	void LoadGameObjectXML(const XMLElement& elem);

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
