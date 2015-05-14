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

struct HitComponent : Component{

	virtual void OnHit(RaycastHit hitInfo, GameObject* sender) = 0;

	virtual ~HitComponent(){}
};

struct FireGun : Component{
	Input* input;
	PhysicsSim* physicsSim;
	SC_Transform* camera;

	FireGun(){
		
	}

	virtual void OnAwake(){
		input = &gameObject->scene->input;
		physicsSim = gameObject->scene->physicsSim;
		camera = gameObject->scene->camera;
	}

	virtual void OnUpdate(){
		if(input->GetMouseUp(GLUT_LEFT_BUTTON)){
			RaycastHit hit = physicsSim->Raycast(camera->GlobalPosition(), camera->Forward());
			if(hit.hit){
				HitComponent* hitComp = hit.col->gameObject->GetComponent<HitComponent>();
				if(hitComp != NULL){
					hitComp->OnHit(hit, gameObject);
				}
			}
		}
	}

	virtual ~FireGun(){}
};

struct MatChangeOnHit : HitComponent{
	virtual void OnHit(RaycastHit hitInfo, GameObject* sender){
		double x = rand();
		double ratio = x/RAND_MAX;
		gameObject->material->SetVec4Uniform("_color", Vector4(ratio,1.0,ratio,1.0));
	}

	virtual ~MatChangeOnHit(){}
};

struct RotateConstantly : Component{
	float rotationSpeed;

	RotateConstantly(){
		rotationSpeed = 30;
	}

	virtual void OnUpdate(){
		gameObject->transform.rotation = gameObject->transform.rotation * Quaternion(Y_AXIS, rotationSpeed * gameObject->scene->deltaTime);
	}
};

struct CameraControl : Component{
	Input* input;
	SC_Transform* camera;
	PhysicsSim* physics;
	float speed;
	float velocity;

	bool isGrounded;

	int prevX;
	int prevY;
	float xRot;
	float yRot;

	CameraControl(){
		speed = 3;
		prevX = 0;
		prevY = 0;
		xRot = 0;
		yRot = 0;
		velocity = 0;
		isGrounded = false;
	}

	virtual void OnAwake(){
		input = &gameObject->scene->input;
		camera = gameObject->scene->camera;
		physics = gameObject->scene->physicsSim;
	}

	virtual void OnUpdate(){
		float deltaX = input->GetMouseX() - prevX;
		float deltaY = input->GetMouseY() - prevY;
	
		xRot = xRot + deltaX;
		yRot = yRot + deltaY;

		camera->rotation = Quaternion(Y_AXIS, xRot/80) * Quaternion(X_AXIS, yRot/80 - 3);

		prevX = input->GetMouseX();
		prevY = input->GetMouseY();
		Vector3 moveVec(0,0,0);
		if(input->GetKey('w')){
			moveVec = moveVec + camera->Forward();
		}
		if(input->GetKey('s')){
			moveVec = moveVec + camera->Forward() * -1; 
		}
		if(input->GetKey('a')){
			moveVec = moveVec + camera->Right() * -1;
		}
		if(input->GetKey('d')){
			moveVec = moveVec + camera->Right();
		}
		if(input->GetKeyDown(' ') && isGrounded){
			velocity = 4;
		}

		moveVec.y = 0;
		if(moveVec.MagnitudeSquared() > 0){
			moveVec = moveVec.Normalized() * gameObject->scene->deltaTime * speed;

			RaycastHit testHit = physics->Raycast(camera->GlobalPosition(), moveVec);
			if(!testHit.hit || testHit.depth > moveVec.Magnitude() + 0.2f){
				camera->GetParent()->position = camera->GetParent()->position + moveVec;
			}
			else if (testHit.hit){
				Vector3 badVec = testHit.normal * DotProduct(moveVec, testHit.normal);
				Vector3 goodVec = moveVec - badVec;
				camera->GetParent()->position = camera->GetParent()->position + goodVec;
			}
		}

		float floorHeight = -10;
		RaycastHit lookDown = physics->Raycast(camera->GetParent()->position, Y_AXIS*-1);
		if(lookDown.hit){
			floorHeight = lookDown.worldPos.y;
		}

		velocity -= gameObject->scene->deltaTime * 5;
		camera->GetParent()->position.y += velocity * gameObject->scene->deltaTime;
		if(camera->GetParent()->position.y <= floorHeight + 1){
			camera->GetParent()->position.y = floorHeight + 1;
			velocity = 0;
			isGrounded = true;
		}
		else{
			isGrounded = false;
		}
	}

	virtual ~CameraControl(){}
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
		if(col->gameObject->transform.GetParent() == col->gameObject->scene->camera){
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
	camera = NULL;
	rb = NULL;
	physicsSim = new PhysicsSim();
	input = Input();

	myRandom = default_random_engine(time(NULL));

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

	//glutDisplayFunc(RenderScene);
	//glutIdleFunc(RenderScene);
	glutMouseFunc(OnMouseFunc);
	//glutMotionFunc(OnPassiveMouseFunc);
	glutPassiveMotionFunc(OnPassiveMouseFunc);
	glutMotionFunc(OnPassiveMouseFunc);
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

GuiElement* Scene::AddGuiElement(){
	GuiElement* elem = new GuiElement(&resources);
	guiElements.push_back(elem);

	return elem;
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
	cout << "Scene::Update(): " << deltaTime * 1000 << " ms.\n";
	//cout << "Camera is at: " << camera->GlobalPosition().x << ", " << camera->GlobalPosition().y << ", " << camera->GlobalPosition().z << endl;

	/*
	float newX = guiElements[0]->position.x + 1.55*deltaTime;
	newX = newX - (int)newX;
	guiElements[0]->position.x = newX;

	float newY = guiElements[0]->position.y + 0.9*deltaTime;
	newY = newY - (int)newY;
	guiElements[0]->position.y = newY;
	*/

	GameObject* parent = (*objects.begin());
	GameObject* child  = (*objects.rbegin());


	clock_t before = clock();
	GuiSetSliderValue(guiElements[0], (1+sin(float(currTime)/divisor))/2);
	clock_t after = clock();
	double sliderTime = ((double)after - before)/CLOCKS_PER_SEC;
	//cout << "Time: " << sliderTime << endl;

	if(child->transform.GetParent() == camera){
		//child->transform.rotation = camera.rotation.Conjugate();
	}

	parent->transform.rotation = parent->transform.rotation * Quaternion(Y_AXIS, deltaTime/3);
	//child->transform.position = child->transform.position + Vector3(2 + sin(((float)prevTime)/1000),0,0);

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		(*iter)->OnUpdate();
	}

	const float speed = 5;

	if(input.GetKeyUp('o')){
		SaveScene("Quicksave.xml");
	}
	if(input.GetKeyUp('l')){
		LoadScene("Quicksave.xml");
	}

	if(input.GetKeyUp('v')){
		float randY = (((double)(myRandom() % RAND_MAX))/RAND_MAX);
		float randZ = (((double)(myRandom() % RAND_MAX))/RAND_MAX);

		
		GameObject* y = new GameObject();
		y->scene = this;
		y->transform.scale = Vector3(0.005f,0.005f,22);
		y->transform.SetParent(camera);
		y->AddMaterial("shader", "Texture.bmp");
		y->AddMesh("test.obj");
		y->AddComponent<ColorizerComp>();
		y->name = "cameraSpawn";

		AddObject(y);

		//The rigidbody gets added to the physics sim, which manages its memory
		BoxCollider* col = new BoxCollider(Vector3(0.0f,0.0f,0.0f), Vector3(0.5f,0.5f,0.5f));
		RigidBody* rbody = new RigidBody(&(y->transform), col);
		rbody->isKinematic = true;
	}

	if(input.GetKey('x')){
		Stop();
	}
}

void PhysicsUpdate(){
	
}

void Scene::OnPostLoad(){
	GuiElement* elem = AddGuiElement();
	elem->tex = new Texture(300, 1);
	elem->position = Vector2(0.1, 0.1);
	elem->scale = Vector2(0.2, 0.02);

	camera->gameObject->AddComponent<FireGun>();
	camera->gameObject->AddComponent<CameraControl>();

	FindGameObject("myObj2_2")->AddComponent<MatChangeOnHit>();
	FindGameObject("wall1")->AddComponent<MatChangeOnHit>();
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

	Mat4x4 camMatrix = camera->GetCameraMatrix();

	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	for(auto iter = drawCalls.rbegin(); iter != drawCalls.rend(); iter++){
		glUniformMatrix4fv(iter->obj->material->GetUniformByName("_perspMatrix"), 1, GL_TRUE, &perspMatrix.m[0][0]);
		glUniformMatrix4fv(iter->obj->material->GetUniformByName("_cameraMatrix"), 1, GL_TRUE,  &camMatrix.m[0][0]);

		iter->Draw();	
	}

	//Gui and gizmos stuff 
	glDisable(GL_DEPTH_TEST);

	Material* col = resources.GetMaterialByName("color");
	glUseProgram(col->shaderProgram); 
	col->SetMat4Uniform("_perspMatrix",  perspMatrix); 
	col->SetMat4Uniform("_cameraMatrix", camMatrix); 
	col->SetVec4Uniform("_color", Vector4(1,1,1,1));
	for(auto iter = physicsSim->staticBoxBodies.begin(); iter != physicsSim->staticBoxBodies.end(); iter++){

		Vector3 min = (*iter)->position - (*iter)->size;
		Vector3 max = (*iter)->position + (*iter)->size;

		//Vector3 minGlobal = (*iter)->gameObject->transform.LocalToGlobal(min);
		//Vector3 maxGlobal = (*iter)->gameObject->transform.LocalToGlobal(max);

		Vector3 corners[8] = {   Vector3(min.x, min.y, min.z),
								 Vector3(min.x, max.y, min.z),
							     Vector3(min.x, max.y, max.z),
								 Vector3(min.x, min.y, max.z),
								 Vector3(max.x, min.y, min.z),
								 Vector3(max.x, max.y, min.z),
								 Vector3(max.x, max.y, max.z),
								 Vector3(max.x, min.y, max.z)};

		for(int i = 0; i < 8; i++){
			corners[i] = (*iter)->gameObject->transform.LocalToGlobal(corners[i]);
		}

		int indices[12] = {0,1,2,3, 7,6,5,4, 0,1,2,3};
		break;

		if((*iter)->gameObject->name == "floor"){
			glBegin(GL_LINE_STRIP);
			{
				for(int i = 0; i < 12; i++){
					glVertex3f(corners[indices[i]].x, corners[indices[i]].y, corners[indices[i]].z);
				}
			}
			glEnd();
		}
	}

	//Gui stuff
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(auto iter = guiElements.begin(); iter != guiElements.end(); iter++){
		(*iter)->OnGui();
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
	input.mouseX = x;
	input.mouseY = y;
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

	for(auto iter = guiElements.begin(); iter != guiElements.end(); iter++){
		delete (*iter);
	}

	guiElements.clear();

	physicsSim->staticBoxBodies.clear();
	physicsSim->staticSphereBodies.clear();
	for(auto iter = physicsSim->dynamicBodies.begin(); iter != physicsSim->dynamicBodies.end(); iter++){
		delete *iter;
	}
	physicsSim->dynamicBodies.clear();

	drawCalls.clear();

	resources.Clear();
}

void Scene::Stop(){
	running = false;
}

Scene::~Scene(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	for(auto iter = guiElements.begin(); iter != guiElements.end(); iter++){
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

