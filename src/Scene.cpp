#include "../header/int/Scene.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Vector4.h"
#include <GL/freeglut.h>
#include <GL/glew.h>

Scene::Scene(){
}

Scene::Scene(int argc, char** argv){
	drawCalls = list<DrawCall>();
	camera = SC_Transform();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("my-engine");

	GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutMouseFunc(OnMouseFunc);
	//glutMotionFunc(OnPassiveMouseFunc);
	glutPassiveMotionFunc(OnPassiveMouseFunc);
	glutKeyboardFunc(OnKeyFunc);
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable (GL_DEPTH_TEST);

	Init();

	xRot = 0;
	yRot = 0;

    printf("GL version: %s\n", glGetString(GL_VERSION));
}

void Scene::Init(){
	GameObject* y = new GameObject();
	y->AddMaterial("shader", "Texture.bmp");
	y->AddMesh("test.obj");
	AddObject(y);
}

GameObject* Scene::AddObject(GameObject* obj){
	objects.push_back(obj);
	obj->scene = this;

	if(obj->material != NULL && obj->mesh != NULL){
		cout << "Added draw call\n";
		drawCalls.push_back(DrawCall(*(obj->mesh), obj->material));
	}

	return obj;
}

void Scene::Start(){
    glutMainLoop();
}

void Scene::UpdateVertexBuffer(){

}

void Scene::OnUpdate(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		(*iter)->OnUpdate();
	}
}

void Scene::Render(){
	clock_t currTime = clock();
	deltaTime = ((float)currTime - prevTime)/CLOCKS_PER_SEC;
	prevTime = currTime;

	float aspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	float fieldOfView = 80;
	float nearZ = 0.1;
	float farZ = 1000;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mat4x4 perspMatrix = GetPerspectiveMatrix(aspectRatio,fieldOfView, nearZ, farZ);

	Mat4x4 camMatrix = camera.GetInverse().LocalToGlobalMatrix();

	Mat4x4 finalMatrix = perspMatrix * camMatrix;

	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		glUniformMatrix4fv(iter->material->objectMatrixUniform, 1, GL_TRUE, &finalMatrix.m[0][0]);
		
		iter->material;
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

	camera.rotation = Quaternion(X_AXIS, yRot/200) * Quaternion(Y_AXIS, xRot/200);

	//camera.rotation = camera.rotation * Quaternion(X_AXIS, deltaY/200) * Quaternion(Y_AXIS, deltaX/200);

	prevX = x;
	prevY = y;
}

void Scene::OnKey(unsigned char key, int x, int y){
	if(key == 'x'){
		Stop();
	}
	else if(key == 'w'){
		camera.position = camera.position - (Z_AXIS * deltaTime * 10);
		//cout << "Delta-time: " << deltaTime << endl;
	}
	else if(key == 's'){
		camera.position = camera.position + (Z_AXIS * deltaTime * 10);
	}
	else if(key == 'a'){
		camera.position = camera.position - (X_AXIS * deltaTime * 10);
	}
	else if(key == 'd'){
		camera.position = camera.position + (X_AXIS * deltaTime * 10);
	}
	else if(key == 'q'){
		camera.position = camera.position + (Y_AXIS * deltaTime * 10);
	}
	else if(key == 'z'){
		camera.position = camera.position - (Y_AXIS * deltaTime * 10);
	}
}

void Scene::Stop(){
	glutLeaveMainLoop();
}

Scene::~Scene(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}
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
