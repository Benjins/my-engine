#include "../header/int/Scene.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Vector4.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/RigidBody.h"
#include "../header/int/Collider.h"
#include "../header/int/PhysicsSim.h"
#include <time.h>
#include <cstdlib>

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

struct TestComp : Component{
	virtual void OnCollision(Collider* col){
		cout << "Collision: " << col->gameObject->name << endl;
	}
};

struct OscillateUp : Component{
	float time;
	int frameCount;

	virtual void OnAwake(){
		time = 0;
		frameCount = 0;
	}

	virtual void OnUpdate(){
		time += gameObject->scene->deltaTime;
		frameCount++;
		gameObject->transform.position.y = sinf(time)*2;

		if(frameCount % 60 == -1){
			GameObject* z2 = new GameObject();
			z2->scene = gameObject->scene;
			//z2->transform.SetParent(gameObject->transform.GetParent());
			z2->transform.position = gameObject->transform.GlobalPosition();
			z2->transform.scale = Vector3(0.15f, 0.15f, 0.15f);
			z2->AddMaterial("shader", "Texture.bmp");
			z2->AddMesh("test.obj");
			z2->AddComponent<BoxCollider>();
			z2->name = "Child_ball_clone";

			gameObject->scene->AddObject(z2);
		}
	}
};

struct ColorizerComp : Component{
	virtual void OnCollision(Collider* col){
		Material* mat = col->gameObject->material;
		if(mat != NULL){
			double x = rand();
			double ratio = x/RAND_MAX;
			mat->SetVec4Uniform("_color",Vector4(0.6,0.6,ratio,1.0));
			for(auto iter = col->gameObject->transform.children.begin(); iter != col->gameObject->transform.children.end(); iter++){
				Material* childMat = (*iter)->gameObject->material;
				childMat->SetVec4Uniform("_color",Vector4(1.0,0,0,1.0));
			}
		}
	}
};

struct ChangeColOnCollision : Component{
	bool collided;
	Texture* tex;

	virtual void OnAwake(){
		collided = false;
		tex = gameObject->material->mainTexture;
	}

	virtual void OnCollision(Collider* col){
		if(col->gameObject->transform.GetParent() == &(col->gameObject->scene->camera)){
			if(gameObject->material != NULL){
				double x = rand();
				double ratio = x/RAND_MAX;

				for(int i = 0; i < 256; i++){
					for(int j = 0; j < 256; j++){
						tex->SetPixel(i,j,ratio,ratio,ratio);
					}
				}

				tex->Apply();
				//gameObject->material->SetVec4Uniform("_color",Vector4(1.0f, (float)ratio,1.0f,1.0f));
			}
		}
	}
};


Scene::Scene(){
}

Scene::Scene(int argc, char** argv){
	drawCalls = list<DrawCall>();
	camera = SC_Transform();
	camera.position = Z_AXIS * -5 + Y_AXIS * 4;
	camera.rotation = QUAT_IDENTITY;//Quaternion(Y_AXIS, 3.14159f);
	rb = NULL;
	physicsSim = new PhysicsSim();
	input = Input();

	cout << "sizeof(Material)" << sizeof(Material) << endl;
	cout << "sizeof(Mesh)" << sizeof(Model) << endl;
	cout << "sizeof(Texture)" << sizeof(Texture) << endl;
	cout << "sizeof(GameObject)" << sizeof(GameObject) << endl;
	cout << "sizeof(BoxCollider)" << sizeof(BoxCollider) << endl;
	cout << "sizeof(PhysicsSim)" << sizeof(PhysicsSim) << endl;

	cout << "PhysicsSim location: " << (size_t)physicsSim << endl;

	myRandom = default_random_engine(time(NULL));

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(500, 100);
    glutCreateWindow("my-engine");

#ifndef __APPLE__
	GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }
#endif

	//glutDisplayFunc(RenderScene);
	//glutIdleFunc(RenderScene);
	glutMouseFunc(OnMouseFunc);
	//glutMotionFunc(OnPassiveMouseFunc);
	glutPassiveMotionFunc(OnPassiveMouseFunc);
	glutKeyboardFunc(OnKeyFunc);
	glutKeyboardUpFunc(OnKeyUpFunc);
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	//Init();

	xRot = 0;
	yRot = 0;

    printf("GL version: %s\n", glGetString(GL_VERSION));
}

void Scene::Init(){
	/*
	GameObject* y = new GameObject();
	y->scene = this;
	y->transform.position = Vector3(0, 2.0f, 0);
	y->AddMaterial("shader", "Texture.bmp");
	y->AddMesh("test.obj");
	BoxCollider* yBox = y->AddComponent<BoxCollider>();
	yBox->size = Vector3(1,1,1);

	y->name = "Rotating_centre";

	AddObject(y);

	//rb = new RigidBody(&(y->transform), new SphereCollider(Vector3(0,0,0), 1.0f));

	//rb->AddForce(Vector3(0,-5,0));

	
	GameObject* z = new GameObject();
	z->scene = this;
	z->transform.position = Vector3(0,-3,0);
	z->AddMaterial("shader", "Texture2.bmp");
	z->AddMesh("floor.obj");
	z->name = "Floor";

	BoxCollider* zBox = z->AddComponent<BoxCollider>();
	zBox->position = Vector3(0,0,0);
	zBox->size = Vector3(8, 0.1f, 8);

	AddObject(z);
	

	GameObject* z2 = new GameObject();
	z2->scene = this;
	z2->transform.SetParent(&(y->transform));
	z2->transform.position = Vector3(3, 0.0f, 0);
	z2->transform.scale = Vector3(0.2f, 0.2f, 0.2f);
	z2->AddMaterial("shader", "Texture.bmp");
	z2->AddMesh("test.obj");
	z2->AddComponent<BoxCollider>();
	z2->name = "Child_ball_clone";

	AddObject(z2);

	GameObject* z3 = new GameObject();
	z3->scene = this;
	z3->transform.SetParent(&(y->transform));
	z3->transform.position = Vector3(-3, 0.0f, 0);
	z3->transform.scale = Vector3(0.2f, 0.2f, 0.2f);
	z3->AddMaterial("shader", "Texture.bmp");
	z3->AddMesh("test.obj");
	z3->AddComponent<BoxCollider>();
	z3->AddComponent<OscillateUp>();
	z3->name = "Child_ball2";

	AddObject(z3);

	GameObject* camChild = new GameObject();
	camChild->scene = this;
	camChild->AddMaterial("shader", "Texture.bmp");
	camChild->AddMesh("test.obj");
	camChild->transform.position = Vector3(0,0,0);
	camChild->transform.scale = Vector3(1,1,1);
	camChild->name = "CamChild";
	camChild->AddComponent<BoxCollider>();

	AddObject(camChild);

	rb = new RigidBody(&(camChild->transform), new BoxCollider(Vector3(0,0,0), Vector3(1,1,1)));

	//camChild->AddComponent<ChangeColOnCollision>();
	*/
}

GameObject* Scene::FindGameObject(string name){
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

void Scene::Start(){
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
	timeval start;
	gettimeofday(&start, NULL);
	prevTime = start.tv_sec*1000 + start.tv_usec/1000;
#else
	prevTime = clock();
#endif

	running = true;
	while(running){
		glutMainLoopEvent();
		OnUpdate();
		physicsSim->Advance(deltaTime);

		Render();
		glutPostRedisplay();

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
	//cout << "Scene::Update(): " << deltaTime << endl;

	GameObject* parent = (*objects.begin());
	GameObject* child  = (*objects.rbegin());

	if(child->transform.GetParent() == &camera){
		//child->transform.rotation = camera.rotation.Conjugate();
	}

	parent->transform.rotation = parent->transform.rotation * Quaternion(Y_AXIS, deltaTime/3);
	//child->transform.position = child->transform.position + Vector3(2 + sin(((float)prevTime)/1000),0,0);

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		(*iter)->OnUpdate();
	}

	const float speed = 5;

	if(input.GetKeyUp('v')){
		RaycastHit hit = physicsSim->Raycast(camera.GlobalPosition(), camera.Forward());
		if(hit.hit){
			double x = rand();
			double ratio = x/RAND_MAX;
			hit.col->gameObject->material->SetVec4Uniform("_color", Vector4(ratio,1.0,ratio,1.0));
		}
	}

	if(input.GetKeyUp('o')){
		SaveScene("Quicksave.xml");
	}
	if(input.GetKeyUp('l')){
		LoadScene("Quicksave.xml");
	}

	if(input.GetMouseUp(GLUT_LEFT_BUTTON)){
		float randY = (((double)(myRandom() % RAND_MAX))/RAND_MAX);
		float randZ = (((double)(myRandom() % RAND_MAX))/RAND_MAX);

		
		GameObject* y = new GameObject();
		y->scene = this;
		y->transform.scale = Vector3(0.005f,0.005f,22);
		y->transform.SetParent(&camera);
		y->AddMaterial("shader", "Texture.bmp");
		y->AddMesh("test.obj");
		y->AddComponent<ColorizerComp>();
		y->name = "cameraSpawn";

		AddObject(y);

		//The rigidbody gets added to the physics sim, which manages its memory
		BoxCollider* col = new BoxCollider(Vector3(0.0f,0.0f,0.0f), Vector3(0.5f,0.5f,0.5f));
		rb = new RigidBody(&(y->transform), col);
		
	}

	if(input.GetKey('x')){
		Stop();
	}
	if(input.GetKey('w')){
		camera.position = camera.position + (camera.Forward() * deltaTime * speed);
	}
	if(input.GetKey('s')){
		camera.position = camera.position - (camera.Forward() * deltaTime * speed);
	}
	if(input.GetKey('a')){
		camera.position = camera.position - (camera.Right() * deltaTime * speed);
	}
	if(input.GetKey('d')){
		camera.position = camera.position + (camera.Right() * deltaTime * speed);
	}
	if(input.GetKey('q')){
		camera.position = camera.position + (Y_AXIS * deltaTime * speed);
	}
	if(input.GetKey('z')){
		camera.position = camera.position - (Y_AXIS * deltaTime * speed);
	}
	if(input.GetKey('e')){
		camera.rotation = camera.rotation * (Quaternion(camera.Forward(), -0.012f));
	}
	if(input.GetKey('c')){
		camera.rotation = camera.rotation * (Quaternion(camera.Forward(), 0.012f));
	}
	if(input.GetKeyUp('i')){
		//camera.Right().Print();
		camera.rotation.Print();
	}
}

void PhysicsUpdate(){
	
}

void Scene::Render(){
	//rb->StepForward(deltaTime);

	GameObject* y = *objects.begin();
	//y->transform.rotation = Quaternion(Y_AXIS, ((float)currTime)/1000);
	//y->transform.position.y = sinf(((float)currTime)/1000);

	float aspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	float fieldOfView = 80;
	float nearZ = 0.1;
	float farZ = 1000;
	                                                                    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mat4x4 perspMatrix = GetPerspectiveMatrix(aspectRatio,fieldOfView, nearZ, farZ);

	Mat4x4 camMatrix = camera.GetCameraMatrix();

	for(auto iter = drawCalls.rbegin(); iter != drawCalls.rend(); iter++){
		glUniformMatrix4fv(iter->obj->material->GetUniformByName("_perspMatrix"), 1, GL_TRUE, &perspMatrix.m[0][0]);
		glUniformMatrix4fv(iter->obj->material->GetUniformByName("_cameraMatrix"), 1, GL_TRUE,  &camMatrix.m[0][0]);

		iter->Draw();	
	}

	glutSwapBuffers();
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
	float deltaX = x - prevX;
	float deltaY = y - prevY;
	
	xRot = xRot + deltaX;
	yRot = yRot + deltaY;

	camera.rotation = Quaternion(Y_AXIS, xRot/80) * Quaternion(X_AXIS, yRot/80 - 3);
	
	//camera.rotation = camera.rotation * (Quaternion(Y_AXIS, deltaX/80) * Quaternion(camera.Right(), deltaY/80));
	//camera.rotation = camera.rotation * Quaternion(X_AXIS, deltaY/200) * Quaternion(Y_AXIS, deltaX/200);
	prevX = x;
	prevY = y;
}

void Scene::OnKey(unsigned char key, int x, int y){
	input.PushKey(key);
}

void Scene::OnKeyUp(unsigned char key, int x, int y){
	input.ReleaseKey(key);
}

void Scene::RemoveAllObjects(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	objects.clear();
	physicsSim->staticBoxBodies.clear();
	physicsSim->staticSphereBodies.clear();
	physicsSim->dynamicBodies.clear();

	drawCalls.clear();
}

void Scene::Stop(){
	running = false;
}

Scene::~Scene(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	delete physicsSim;
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

