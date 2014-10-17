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

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutMouseFunc(OnMouseFunc);
	glutMotionFunc(OnPassiveMouseFunc);
	glutKeyboardFunc(OnKeyFunc);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }
    
	GameObject* y = new GameObject();
	y->AddMaterial("shader", "Texture.bmp");
	y->AddMesh("test.obj");
	AddObject(y);

    printf("GL version: %s\n", glGetString(GL_VERSION));

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable (GL_DEPTH_TEST);

    glutMainLoop();
}

GameObject* Scene::AddObject(GameObject* obj){
	objects.push_back(obj);
	obj->scene = this;

	if(obj->material != NULL && obj->mesh != NULL){
		drawCalls.push_back(DrawCall(*(obj->mesh), obj->material));
	}

	return obj;
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
	Mat4x4 objectMatrix = GetPerspectiveMatrix(aspectRatio,fieldOfView, nearZ, farZ);


	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		//Mat4x4 cameraMatrix = camera.GetInverse().LocalToGlobalMatrix();

		//glUniformMatrix4fv(iter->material->cameraUniform, 1, GL_TRUE, &objectMatrix.m[0][0]);
		
		iter->material;
		iter->Draw();	
	}

	glutSwapBuffers();
}

void Scene::OnMouse(int button, int state, int x, int y){
	if(state == GLUT_DOWN){
		cout << "Button down in callback.\n";
		//int deltaX = x - prevX/2;
		//int deltaY = y - prevY/2;
		//camera.rotation = camera.rotation * Quaternion(X_AXIS,deltaX) * Quaternion(Y_AXIS, deltaY);
	}
	
	prevX = x;
	prevY = y;

	//glutWarpPointer	(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
}

void Scene::OnPassiveMouse(int x, int y){
	//if(buttonDown >= 0){
		//cout << "Button down in callback.\n";
		float deltaX = x - prevX/2;
		float deltaY = y - prevY/2;
		camera.rotation = camera.rotation * Quaternion(X_AXIS,deltaX/100) * Quaternion(Y_AXIS, deltaY/100);
	//}

	//prevX = x;
	//prevY = y;
	//int deltaX = x - glutGet(GLUT_WINDOW_WIDTH)/2;
	//int deltaY = y - glutGet(GLUT_WINDOW_HEIGHT)/2;

	//camera.rotation = camera.rotation * Quaternion(X_AXIS,deltaX) * Quaternion(Y_AXIS, deltaY);
}

void Scene::OnKey(unsigned char key, int x, int y){
	if(key == 'q'){
		glutLeaveMainLoop();
	}
	else if(key == 'w'){
		camera.position = camera.position + camera.Forward() * deltaTime;
		cout << "Delta-time: " << deltaTime << endl;
	}
	else if(key == 's'){
		camera.position = camera.position - camera.Forward() * deltaTime;
	}
}

Scene::~Scene(){
	glutLeaveMainLoop();

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
