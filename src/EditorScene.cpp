#include "../header/int/EditorScene.h"
#include "../header/int/Input.h"
#include "../header/int/PhysicsSim.h"

EditorScene::EditorScene(int argc, char** argv) : Scene(argc, argv){
	glutMouseFunc(OnEditorMouseFunc);
	glutPassiveMotionFunc(OnEditorPassiveMouseFunc);
	glutMotionFunc(OnEditorPassiveMouseFunc);
	glutKeyboardFunc(OnEditorKeyFunc);
	glutKeyboardUpFunc(OnEditorKeyUpFunc);
}

void EditorScene::Start(){
	
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
	timeval start;
	gettimeofday(&start, NULL);
	prevTime = start.tv_sec*1000 + start.tv_usec/1000;
#else
	prevTime = clock();
#endif


	editorCamera.position = Vector3(3,3,-3);
	editorCamera.rotation = Quaternion(Y_AXIS, -45) * Quaternion(X_AXIS, 45);
	camera = &editorCamera;

	running = true;
	while(running){
		glutMainLoopEvent();

		EditorUpdate();

		Render();
		glutPostRedisplay();

		input.EndFrame();
	}
}

void EditorScene::EditorUpdate(){
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
	
	//cout << "EditorScene::EditorUpdate(): " << (deltaTime * 1000) << "  ms.\n";

	if(input.GetKeyUp('x')){
		Stop();
	}

	const float speed = 2.0f;

	if(input.GetKey('w')){
		editorCamera.position = editorCamera.position + editorCamera.Forward() * speed * deltaTime;
	}
	if(input.GetKey('s')){
		editorCamera.position = editorCamera.position - editorCamera.Forward() * speed * deltaTime;
	}
	if(input.GetKey('a')){
		editorCamera.position = editorCamera.position - editorCamera.Right() * speed * deltaTime;
	}
	if(input.GetKey('d')){
		editorCamera.position = editorCamera.position + editorCamera.Right() * speed * deltaTime;
	}
	if(input.GetKey('q')){
		editorCamera.position = editorCamera.position + Y_AXIS * speed * deltaTime;
	}
	if(input.GetKey('z')){
		editorCamera.position = editorCamera.position - Y_AXIS * speed * deltaTime;
	}

	if(input.GetMouse(GLUT_RIGHT_BUTTON)){
		float deltaX = prevX - input.mouseX;
		float deltaY = prevY - input.mouseY;

		xRot += deltaY/90;
		yRot += deltaX/90;

		editorCamera.rotation =  Quaternion(Y_AXIS, yRot) * Quaternion(X_AXIS, xRot);
	}

	prevX = input.mouseX;
	prevY = input.mouseY;
}

static void OnEditorMouseFunc(int button, int state, int x, int y){
	EditorScene::getInstance().OnMouse(button, state, x, y);
}

static void OnEditorPassiveMouseFunc(int x, int y){
	EditorScene::getInstance().OnPassiveMouse(x, y);
}

static void OnEditorKeyFunc(unsigned char key, int x, int y){
	EditorScene::getInstance().OnKey(key, x, y);
}

static void OnEditorKeyUpFunc(unsigned char key, int x, int y){
	EditorScene::getInstance().OnKeyUp(key, x, y);
}