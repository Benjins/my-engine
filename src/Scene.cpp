#include "../header/int/Scene.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Vector4.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/RigidBody.h"
#include "../header/int/Collider.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/Armature.h"
#include "../header/int/CubeMap.h"
#include "../header/int/FontBMPMaker.h"
#include "../header/ext/simple-xml.h"
#include <time.h>
#include <cstdlib>
#include <algorithm>

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
#include <sys/time.h>
#endif 

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include "../header/int/UserComps.h"

Scene::Scene(){
}

Scene::Scene(int argc, char** argv){
	drawCalls = vector<DrawCall>();
	camera = NULL;
	physicsSim = new PhysicsSim();
	input = Input();
	pathfinding.scene = this;

	skyBox = nullptr;

	myRandom = default_random_engine(time(NULL));

	audio.Initialise();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_ALPHA);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(500, 100);
    glutCreateWindow("my-engine");

#ifndef __APPLE__
	GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }
#endif

	glutMouseFunc(OnMouseFunc);
	glutPassiveMotionFunc(OnPassiveMouseFunc);
	glutMotionFunc(OnPassiveMouseFunc);
	glutKeyboardFunc(OnKeyFunc);
	glutKeyboardUpFunc(OnKeyUpFunc);
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	xRot = 0;
	yRot = 0;

    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("GL vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
}

void Scene::Init(){
}

GameObject* Scene::FindGameObject(const string& name){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		GameObject* obj = *iter;
		if(obj->name == name){
			return obj;
		}
	}

	return NULL;
}

GameObject* Scene::AddObject(GameObject* obj){
	objects.push_back(obj);
	obj->scene = this;

	if(obj->material != NULL && obj->mesh != NULL){
		drawCalls.push_back(DrawCall(obj));
	}

	return obj;
}

GameObject* Scene::AddObjectAndDescendants(GameObject* obj){
	AddObject(obj);
	for(SC_Transform* child : obj->transform.children){
		AddObjectAndDescendants(child->gameObject);
	}

	return obj;
}

void Scene::RemoveObject(GameObject* obj){
	if(std::find(destroyedObjects.begin(), destroyedObjects.end(), obj) != destroyedObjects.end()){
		//cout << "\n\nWarning: Tried to remove the same gameobject twice.  Subsequent calls ignored.\n";
	}
	else{
		destroyedObjects.push_back(obj);
	}
}

void Scene::DestroyObject(GameObject* obj){
	//We remove the child from its parent, which causes issue if we do that while iterating over the children list
	vector<GameObject*> childrenCopy; 
	for(SC_Transform* child : obj->transform.children){
		childrenCopy.push_back(child->gameObject);
	}

	//Remove parent's reference to us
	obj->transform.SetParent(nullptr);

	for(GameObject* child : childrenCopy){
		DestroyObject(child);
	}

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		if(*iter == obj){
			objects.erase(iter);
			delete obj;
			break;
		}
	}

	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		if((*iter).obj == obj){
			drawCalls.erase(iter);
			break;
		}
	}
}

int Scene::AddLight(){
	int newId = 0;
	if(lights.size() > 0){
		newId = lights[lights.size() - 1].id + 1;
	}

	Light newLight = {};
	newLight.id = newId;
	lights.push_back(newLight);

	return newId;
}

void Scene::Start(){
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
	timeval start;
	gettimeofday(&start, NULL);
	prevTime = start.tv_sec*1000 + start.tv_usec/1000;
#else
	prevTime = clock();
#endif

	prevDeltaTimeIndex = 0;
	for(int i = 0; i < FPS_SMOOTHING_FACTOR; i++){
		prevDeltaTime[i] = 0.0f;
	}

	running = true;
	while(running){
		clock_t start = clock();
#ifdef __APPLE__
		glutCheckLoop();
#else
		glutMainLoopEvent();
#endif
		clock_t afterCheck = clock();

		double loopEventTime = ((double)afterCheck - start)/CLOCKS_PER_SEC;

		Render();

		clock_t afterRender = clock();
		double renderTime = ((double)afterRender - afterCheck)/CLOCKS_PER_SEC;

		OnUpdate();
		physicsSim->Advance(deltaTime);

		clock_t afterUpdate = clock();

		double updateTime = ((double)afterUpdate - afterRender)/CLOCKS_PER_SEC;

		//glFinish();

		clock_t afterFinish = clock();
		double finishTime = ((double)afterFinish - afterUpdate)/CLOCKS_PER_SEC;

		glutSwapBuffers();

		clock_t afterSwap = clock();
		double swapTime = ((double)afterSwap - afterFinish)/CLOCKS_PER_SEC;

		double total = ((double)afterSwap - start)/CLOCKS_PER_SEC;

		/*
		if(total < 0.016f){
			sleep(16 - int(total * 1000));
		}
		*/

		printf("event: %.1f, updt: %.1f, rend: %.1f, flush: %.1f, swap: %.1f, total: %.1f\n", loopEventTime * 1000, 1000 * updateTime, renderTime * 1000, finishTime * 1000, swapTime * 1000, total * 1000);

		GLenum err = glGetError();
		if(err != 0){
			printf("GL Error: %d, meaning: '%s'\n", err, gluErrorString(err));
		}

		input.EndFrame();
	}
}

void Scene::UpdateVertexBuffer(){

}

void Scene::OnUpdate(){
	int divisor = CLOCKS_PER_SEC;
	clock_t currTime;
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
	divisor = 1000;
	timeval start;
	gettimeofday(&start, NULL);
	currTime = start.tv_sec*1000 + start.tv_usec/1000;
#else
	currTime = clock();
#endif
	deltaTime = ((double)currTime - prevTime)/divisor;
	prevTime = currTime;

	//cout << "Frame took " << (deltaTime * 1000) << " ms.\n";

	prevDeltaTime[prevDeltaTimeIndex] = deltaTime;
	prevDeltaTimeIndex = (prevDeltaTimeIndex + 1) % FPS_SMOOTHING_FACTOR;

	float smoothedFPS = 0.0f;
	for(int i = 0; i < FPS_SMOOTHING_FACTOR; i++){
		smoothedFPS = smoothedFPS + prevDeltaTime[i];
	}

	smoothedFPS /= FPS_SMOOTHING_FACTOR;

	GuiText* fpsText = static_cast<GuiText*>(guiSystem.FindGUIElement("fpsText"));
	if(fpsText != nullptr && deltaTime > 0){
		string fps = "FPS: " + to_string((int)((1/smoothedFPS) + 0.5f));
		fpsText->text = fps;
	}

	audio.SetListenerPos(camera->GlobalPosition());

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		(*iter)->OnUpdate();
	}

	for(GameObject* obj : spawnedObjects){
		AddObjectAndDescendants(obj);
	}

	spawnedObjects.clear();

	for(GameObject* obj : destroyedObjects){
		DestroyObject(obj);
	}

	destroyedObjects.clear();

	if(input.GetKeyUp('o')){
		SaveScene("Quicksave.xml");
	}
	if(input.GetKeyUp('l')){
		LoadScene("Quicksave.xml");
	}

	if(input.GetKey('x')){
		Stop();
	}
}

void PhysicsUpdate(){
	
}

void Scene::OnPostLoad(){
	//FindGameObject("reticle")->material->SetVec4Uniform("_color", Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	pathfinding.HookUpNodes();
}

void Scene::Render(){
	float aspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	float fieldOfView = 90;
	float nearZ = 0.01;
	float farZ = 1000;
	                                                                    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mat4x4 perspMatrix = GetPerspectiveMatrix(aspectRatio,fieldOfView, nearZ, farZ);

	Mat4x4 camMatrix = camera->GetCameraMatrix();

	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	Vector3 lightVectors[6];
	GLint isDirectional[6];

	for(int i = 0; i < lights.size(); i++){
		lightVectors[i] = lights[i].isDirectional ? lights[i].direction : lights[i].position;
		isDirectional[i] = lights[i].isDirectional ? 1 : 0;
	}
	
	
	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		Vector3 objPos = iter->obj->transform.GlobalPosition();
		Vector3 cameraPos = camera->GlobalPosition();
		Vector3 cameraForward = camera->Forward();

		//if(DotProduct(objPos - cameraPos, cameraForward) > 0){
			glUseProgram(iter->obj->material->shaderProgram);

			Material* mat = iter->obj->material;
			mat->SetMat4Uniform("_perspMatrix", perspMatrix);
			mat->SetMat4Uniform("_cameraMatrix", camMatrix);

			glUniform1f(mat->GetUniformByName("numLights"), lights.size());
			glUniform3fv(mat->GetUniformByName("lightVectors"), lights.size(), (GLfloat*)lightVectors);
			glUniform1iv(mat->GetUniformByName("lightIsDirectional"), lights.size(), isDirectional);

			GLint cubeMapLoc = mat->GetUniformByName("_cubeMap");
			if(cubeMapLoc != -1){
				skyBox->Bind(GL_TEXTURE2);
				glUniform1i(cubeMapLoc, 2);
			}

			iter->Draw();	
		//}
	}
	

	if(skyBox != nullptr){
		skyBox->Render(camMatrix, perspMatrix);
	}

	
	//Gui and gizmos stuff 
	glDisable(GL_DEPTH_TEST);

	//Gui stuff
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	guiSystem.RenderGui();
}

void Scene::OnMouse(int button, int state, int x, int y){
	if(state == GLUT_DOWN){
		input.PushMouse(button);
	}
	else if(state == GLUT_UP){
		input.ReleaseMouse(button);
	}
}

void Scene::OnPassiveMouse(int x, int y){
	input.mouseX = x;
	input.mouseY = y;
}

void Scene::OnKey(unsigned char key, int x, int y){
	input.PushKey(key);
}

void Scene::OnKeyUp(unsigned char key, int x, int y){
	input.ReleaseKey(key);
}

GameObject* Scene::FindPrefab(const string& name){
	for(GameObject* prefab : prefabs){
		if(prefab->name == name){
			return prefab;
		}
	}

	return nullptr;
}

GameObject* Scene::Instantiate(GameObject* obj, Vector3 position, Quaternion rotation /*= QUAT_IDENTITY*/){
	GameObject* instance = obj->Clone();

	instance->transform.position = position;
	instance->transform.rotation = rotation;

	spawnedObjects.push_back(instance);
	return instance;
}

void Scene::RemoveAllObjects(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	objects.clear();

	for(auto iter = prefabs.begin(); iter != prefabs.end(); iter++){
		delete (*iter);
	}

	prefabs.clear();

	for(auto iter = guiSystem.elements.begin(); iter != guiSystem.elements.end(); iter++){
		delete (*iter);
	}

	guiSystem.elements.clear();

	physicsSim->staticBoxBodies.clear();
	physicsSim->staticSphereBodies.clear();
	for(auto iter = physicsSim->dynamicBodies.begin(); iter != physicsSim->dynamicBodies.end(); iter++){
		delete *iter;
	}
	physicsSim->dynamicBodies.clear();

	drawCalls.clear();

	resources.Clear();

	audio.ClearClips();

	if(skyBox != nullptr){
		delete skyBox;
		skyBox = nullptr;
	}
}

void Scene::Stop(){
	running = false;
}

Scene::~Scene(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	for(GameObject* prefab : prefabs){
		delete prefab;
	}

	delete physicsSim;

	if(skyBox != nullptr){
		delete skyBox;
		skyBox = nullptr;
	}

	//delete testArmature;
}

static void RenderScene(){
}

static void OnMouseFunc(int button, int state, int x, int y){
	Scene::getInstance().OnMouse(button, state, x, y);
}

static void OnPassiveMouseFunc(int x, int y){
	Scene::getInstance().OnPassiveMouse(x, y);
}

static void OnKeyFunc(unsigned char key, int x, int y){
	Scene::getInstance().OnKey(key, x, y);
}

Mat4x4 GetPerspectiveMatrix(float aspectRatio, float fieldOfView, float nearZ, float farZ){
	Mat4x4 persp;

	float fieldOfView_Rad = fieldOfView/180*3.14159265f;
	float tanHalfFOV = tanf(fieldOfView_Rad/2);
	float zRange = nearZ - farZ;

	float x1 = 1/aspectRatio/tanHalfFOV;
	float y2 = 1/tanHalfFOV;
	float z3 = (-nearZ - farZ)/zRange;
	float z4 = 2*farZ*nearZ/zRange;
	
	persp.SetRow(0, Vector4(x1,0,0,0));
	persp.SetRow(1, Vector4(0,y2,0,0));
	persp.SetRow(2, Vector4(0,0,z3,z4));
	persp.SetRow(3, Vector4(0,0,1,0));
	
	return persp;
}

static void OnKeyUpFunc(unsigned char key, int x, int y){
	Scene::getInstance().OnKeyUp(key, x, y);
}

