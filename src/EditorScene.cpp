#include "../header/int/EditorScene.h"
#include "../header/int/Input.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/Collider.h"
#include "../header/int/Material.h"
#include "../header/int/Vector4.h"
#include "../header/int/Mat4.h"

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

	float windowWidth  = glutGet(GLUT_WINDOW_WIDTH);
	float windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	float mouseXNormalized = (input.mouseX -  windowWidth/2) / windowWidth  * 2;
	float mouseYNormalized = (input.mouseY - windowHeight/2) / windowHeight * 2;

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
	else if(input.GetMouse(GLUT_LEFT_BUTTON)){

		Vector3 mouseWorldPos = editorCamera.position;
		mouseWorldPos = mouseWorldPos - (editorCamera.Up()    * mouseYNormalized * sinf(80 * DEG2RAD));
		mouseWorldPos = mouseWorldPos + (editorCamera.Right() * mouseXNormalized * (windowWidth/windowHeight) * sinf(80 * DEG2RAD));
		mouseWorldPos = mouseWorldPos + (editorCamera.Forward() * 1.0f);

		Vector3 rayDirection = mouseWorldPos - editorCamera.position;

		RaycastHit testHit = physicsSim->Raycast(editorCamera.position, rayDirection);
		if(testHit.hit){
			if(testHit.col->gameObject->material != nullptr){
				double randVal = ((double)(rand() % 1000))/1000;
				//cout << "Depth: " << testHit.depth << endl;
				//testHit.col->gameObject->transform.position.y += (randVal < 0.5f ? 0.02f : -0.02f);
				FindGameObject("reticle")->transform.SetParent(nullptr);
				FindGameObject("reticle")->transform.position = testHit.worldPos;
			}
		}
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