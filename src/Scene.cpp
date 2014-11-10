#include "../header/int/Scene.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Vector4.h"
#include "../header/int/Collider.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/RigidBody.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

Scene::Scene(){
}

Scene::Scene(int argc, char** argv){
	drawCalls = list<DrawCall>();
	camera = SC_Transform();
	camera.position = Z_AXIS * -5;
	camera.rotation = Quaternion(Y_AXIS, 3.14159f);
	physicsSim = new PhysicsSim();
	input = Input();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("my-engine");

#ifndef __APPLE__
	GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }
#endif

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutMouseFunc(OnMouseFunc);
	//glutMotionFunc(OnPassiveMouseFunc);
	glutPassiveMotionFunc(OnPassiveMouseFunc);
	glutKeyboardFunc(OnKeyFunc);
	glutKeyboardUpFunc(OnKeyUpFunc);
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable (GL_DEPTH_TEST);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	//Init();

	xRot = 0;
	yRot = 0;

    printf("GL version: %s\n", glGetString(GL_VERSION));
}

void Scene::Init(){
	GameObject* y = new GameObject();
	y->scene = this;
	y->transform.position = Vector3(0, 0.0f, 0);
	y->AddMaterial("shader", "Texture.bmp");
	y->AddMesh("test.obj");

	AddObject(y);

	rb = new RigidBody(&(y->transform), new SphereCollider(Vector3(0,0,0), 1.0f));

	rb->AddForce(Vector3(0,-5,0));

	GameObject* z = new GameObject();
	z->transform.position = Vector3(0,-3,0);
	z->AddMaterial("shader", "Texture.bmp");
	Model* model = new Model();

	model->vertices.push_back(Vector3(-5,0,-5));
	model->vertices.push_back(Vector3(-5,0, 5));
	model->vertices.push_back(Vector3( 5,0,-5));
	model->vertices.push_back(Vector3( 5,0, 5));

	model->faces.push_back(Face(0,1,2));
	model->faces.push_back(Face(3,2,1));

	model->faces[0].uv0 = Vector2(-1,-1);
	model->faces[0].uv1 = Vector2(-1, 1);
	model->faces[0].uv2 = Vector2( 1,-1);

	model->faces[1].uv0 = Vector2( 1, 1);
	model->faces[1].uv1 = Vector2( 1,-1);
	model->faces[1].uv2 = Vector2(-1, 1);
	z->mesh = model;
	
	z->scene = this;

	BoxCollider* zBox = z->AddComponent<BoxCollider>();
	zBox->position = Vector3(0,0,0);
	zBox->size = Vector3(5, 0.1f, 5);

	AddObject(z);
}

GameObject* Scene::AddObject(GameObject* obj){
	objects.push_back(obj);
	obj->scene = this;

	if(obj->material != NULL && obj->mesh != NULL){
		cout << "Added draw call\n";
		drawCalls.push_back(DrawCall(obj));
	}

	return obj;
}

void Scene::Start(){
	running = true;
	while(running){
		physicsSim->Advance(deltaTime);
		glutPostRedisplay();
		glutMainLoopEvent();
		OnUpdate();
		input.EndFrame();
	}
}

void Scene::UpdateVertexBuffer(){

}

void Scene::OnUpdate(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		(*iter)->OnUpdate();
	}

	const float speed = 5;

	if(input.GetKey('x')){
		Stop();
	}
	if(input.GetKey('w')){
		Vector3 camForward = Rotate(Z_AXIS, Quaternion(Y_AXIS, xRot/80));
		camForward.x *= -1;
		camera.position = camera.position + (camForward * deltaTime * speed);
	}
	if(input.GetKey('s')){
		Vector3 camForward = Rotate(Z_AXIS, Quaternion(Y_AXIS, xRot/80));
		camForward.x *= -1;
		camera.position = camera.position - (camForward * deltaTime * speed);
	}
	if(input.GetKey('a')){
		Vector3 camRight = Rotate(X_AXIS, Quaternion(Y_AXIS, xRot/80));
		camRight.z *= -1;
		camera.position = camera.position - (camRight * deltaTime * speed);
	}
	if(input.GetKey('d')){
		Vector3 camRight = Rotate(X_AXIS, Quaternion(Y_AXIS, xRot/80));
		camRight.z *= -1;
		camera.position = camera.position + (camRight * deltaTime * speed);
	}
	if(input.GetKey('q')){
		camera.position = camera.position + (Y_AXIS * deltaTime * speed);
	}
	if(input.GetKey('z')){
		camera.position = camera.position - (Y_AXIS * deltaTime * speed);
	}
}

void PhysicsUpdate(){
	
}

void Scene::Render(){
	clock_t currTime = clock();
	deltaTime = ((float)currTime - prevTime)/CLOCKS_PER_SEC;
	prevTime = currTime;

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

	Mat4x4 camMatrix = camera.GetInverse().LocalToGlobalMatrix();

	//Mat4x4 finalMatrix = perspMatrix * camMatrix;

	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		glUniformMatrix4fv(iter->obj->material->GetUniformByName("_perspMatrix"), 1, GL_TRUE, &perspMatrix.m[0][0]);
		glUniformMatrix4fv(iter->obj->material->GetUniformByName("_cameraMatrix"), 1, GL_TRUE,  &camMatrix.m[0][0]);

		iter->Draw();	
	}

	glutSwapBuffers();
}

void Scene::OnMouse(int button, int state, int x, int y){
	if(state == GLUT_DOWN){
		//cout << "Button down in callback.\n";
	}
	
	//prevX = x;
	//prevY = y;
}

void Scene::OnPassiveMouse(int x, int y){
	float deltaX = x - prevX;
	float deltaY = y - prevY;
	
	xRot = xRot + deltaX;
	yRot = yRot + deltaY;

	camera.rotation = Quaternion(Y_AXIS, xRot/80) * Quaternion(X_AXIS, yRot/80);

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
	Scene::getInstance().Render();
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

