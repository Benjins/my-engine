#include "../header/int/EditorScene.h"
#include "../header/int/UserComps.h"
#include "../header/int/Input.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/Collider.h"
#include "../header/int/Texture.h"
#include "../header/ext/EasyBMP.h"
#include "../header/int/Material.h"
#include "../header/int/Vector4.h"
#include "../header/int/Mat4.h"

EditorScene::EditorScene(int argc, char** argv) : Scene(argc, argv){
	glutMouseFunc(OnEditorMouseFunc);
	glutPassiveMotionFunc(OnEditorPassiveMouseFunc);
	glutMotionFunc(OnEditorPassiveMouseFunc);
	glutKeyboardFunc(OnEditorKeyFunc);
	glutKeyboardUpFunc(OnEditorKeyUpFunc);

	selectedObj = nullptr;
	selectedAxis = -1;
	globalManipulator = false;
}

void EditorScene::Start(){
	
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
	timeval start;
	gettimeofday(&start, NULL);
	prevTime = start.tv_sec*1000 + start.tv_usec/1000;
#else
	prevTime = clock();
#endif

	RecalculateSelectionSim();

	sceneCamera = camera;

	editorCamera.position = Vector3(3,3,-3);
	editorCamera.rotation = Quaternion(Y_AXIS, -45) * Quaternion(X_AXIS, 45);
	camera = &editorCamera;

	
	GuiElement* panel = new GuiElement(&resources);
	panel->position = Vector2(0.9f,0.9f);
	panel->scale = Vector2(0.25f, 0.2f);
	panel->tex = new Texture(1,1);
	RGBApixel pix = {50,50,50,220};
	panel->tex->SetPixel(0,0,pix);
	panel->tex->Apply();

	editorGui.push_back(panel);
	
	GuiText* posTxt = new GuiText(&resources, "arial_16.fuv");
	posTxt->position = Vector2(1.55f,-0.8f);
	posTxt->scale = Vector2(1,1);
	posTxt->text = "Position: ";

	editorGui.push_back(posTxt);

	GuiText* meshTxt = new GuiText(&resources, "arial_16.fuv");
	meshTxt->position = Vector2(1.55f,-0.72f);
	meshTxt->scale = Vector2(1,1);
	meshTxt->text = "Mesh file: ";

	editorGui.push_back(meshTxt);

	running = true;
	while(running){
		glutMainLoopEvent();

		EditorUpdate();

		Render();
		EditorGUI();
		glutSwapBuffers();
		glutPostRedisplay();

		input.EndFrame();
	}
}

void EditorScene::RecalculateSelectionSim(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		Vector3 maxVert = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX), minVert = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		GameObject* obj = *iter;
		if(obj->mesh != NULL){
			for(auto iter = obj->mesh->vertices.begin(); iter != obj->mesh->vertices.end(); iter++){
				maxVert.x = max(maxVert.x, iter->position.x);
				minVert.x = min(minVert.x, iter->position.x);

				maxVert.y = max(maxVert.y, iter->position.y);
				minVert.y = min(minVert.y, iter->position.y);

				maxVert.z = max(maxVert.z, iter->position.z);
				minVert.z = min(minVert.z, iter->position.z);
			}
		}

		if(obj->mesh != NULL && obj->mesh->vertices.size() > 0){
			Vector3 center = (maxVert + minVert) / 2;
			Vector3 size   = (maxVert - minVert) / 2;

			BoxCollider* boundingBox = new BoxCollider(center, size);
			boundingBox->gameObject = obj;
			selectionSim.staticBoxBodies.push_back(boundingBox);
		}
		else{
			BoxCollider* boundingBox = new BoxCollider(Vector3(0,0,0), Vector3(0.4f, 0.4f, 0.4f));
			boundingBox->gameObject = obj;
			selectionSim.staticBoxBodies.push_back(boundingBox);
		}
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

	if(input.GetKeyDown('g')){
		globalManipulator = !globalManipulator;
	}

	if(input.GetKeyUp('o')){
		camera = sceneCamera;
		SaveScene("Editor_Scene.xml");
		camera = &editorCamera;
	}
	if(input.GetKeyUp('l')){
		selectedObj=nullptr;
		for(BoxCollider* col : selectionSim.staticBoxBodies){
			delete col;
		}
		selectionSim.staticBoxBodies.clear();
		LoadScene("Editor_Scene.xml");
		RecalculateSelectionSim();
		camera = &editorCamera;
	}

	Vector3 mouseWorldPos = editorCamera.position;
	mouseWorldPos = mouseWorldPos - (editorCamera.Up()    * mouseYNormalized);
	mouseWorldPos = mouseWorldPos + (editorCamera.Right() * mouseXNormalized * (windowWidth/windowHeight));
	mouseWorldPos = mouseWorldPos + (editorCamera.Forward() * 1.2f);

	Vector3 rayDirection = mouseWorldPos - editorCamera.position;

	if(input.GetMouseDown(GLUT_LEFT_BUTTON)){
		if(selectedObj != nullptr){
			Vector3 cameraToObj = selectedObj->transform.GlobalPosition() - editorCamera.position;
			float projectionAmount = DotProduct(cameraToObj, rayDirection)/DotProduct(rayDirection, rayDirection);
			Vector3 projectedCenter = rayDirection * projectionAmount;

			Vector3 orthoPart = projectedCenter - cameraToObj;
			if(orthoPart.MagnitudeSquared() < 1){
				Vector3 rightVector   = globalManipulator ?  X_AXIS : selectedObj->transform.Right();
				Vector3 upVector      = globalManipulator ?  Y_AXIS : selectedObj->transform.Up();
				Vector3 forwardVector = globalManipulator ?  Z_AXIS : selectedObj->transform.Forward();

				float xBit = DotProduct(orthoPart, rightVector);
				float yBit = DotProduct(orthoPart, upVector);
				float zBit = DotProduct(orthoPart, forwardVector);

				if(xBit > yBit && xBit > zBit){
					selectedAxis = 0;
				}
				else if(yBit > xBit && yBit > zBit){
					selectedAxis = 1;
				}
				else if(zBit > yBit && zBit > xBit){
					selectedAxis = 2;
				}
			}
		}
	}

	if(input.GetMouse(GLUT_LEFT_BUTTON) && selectedObj != nullptr && selectedAxis >= 0){
		float deltaX = prevX - input.mouseX;
		float deltaY = prevY - input.mouseY;

		Vector3 rightVector   = globalManipulator ?  X_AXIS : selectedObj->transform.Right();
		Vector3 upVector      = globalManipulator ?  Y_AXIS : selectedObj->transform.Up();
		Vector3 forwardVector = globalManipulator ?  Z_AXIS : selectedObj->transform.Forward();

		const Vector3 axes[3] = {rightVector, upVector, forwardVector};
		Vector3 axis = axes[selectedAxis];

		Vector3 cameraToObj = selectedObj->transform.GlobalPosition() - editorCamera.position;
		float projectionAmount = DotProduct(cameraToObj, rayDirection)/DotProduct(rayDirection, rayDirection);
		Vector3 projectedCenter = rayDirection * projectionAmount;
		Vector3 orthoPart = projectedCenter - cameraToObj;

		float amount = DotProduct(orthoPart, axis);

		selectedObj->transform.position = selectedObj->transform.position + axis * amount;

	}

	if(input.GetMouse(GLUT_RIGHT_BUTTON)){
		float deltaX = prevX - input.mouseX;
		float deltaY = prevY - input.mouseY;

		xRot += deltaY/90;
		yRot += deltaX/90;

		editorCamera.rotation =  Quaternion(Y_AXIS, yRot) * Quaternion(X_AXIS, xRot);
	}
	else if(input.GetMouseUp(GLUT_LEFT_BUTTON)){
		RaycastHit testHit = selectionSim.Raycast(editorCamera.position, rayDirection);
		if(testHit.hit){
			selectedObj = testHit.col->gameObject;
		}
		else{
			selectedObj = nullptr;
		}

		selectedAxis = -1;
	}
	
	if(selectedObj){
		Vector3 position = selectedObj->transform.position;
		static_cast<GuiText*>(editorGui[1])->text = "Position: " + to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
		GuiText* meshTxt = static_cast<GuiText*>(editorGui[2]);
		if(selectedObj->mesh != nullptr){
			meshTxt->text = "Mesh: " + selectedObj->mesh->fileName;
		}
		else{
			meshTxt->text = "Mesh: ";
		}
	}
	else{
		static_cast<GuiText*>(editorGui[1])->text = "Position:";
		static_cast<GuiText*>(editorGui[2])->text = "";
	}

	for(GameObject* obj : objects){
		for(Component* comp : obj->components){
			comp->OnEditorUpdate();
		}
	}

	prevX = input.mouseX;
	prevY = input.mouseY;
}

void EditorScene::EditorGUI(){

	Material* guiMat = resources.GetMaterialByName("gui");
	Material* vertColMat = resources.GetMaterialByName("color");

	if(selectedObj != nullptr){
		glDisable(GL_DEPTH_TEST);
		glLineWidth(5);
		glUseProgram(vertColMat->shaderProgram);

		float width = glutGet(GLUT_WINDOW_WIDTH);
		float height = glutGet(GLUT_WINDOW_HEIGHT);
		float aspectRatio = width/height;
		float fov = 80;
		float nearClip = 0.01f;
		float farClip = 1000;

		//vertColMat->SetMat4Uniform("_perspMatrix", GetPerspectiveMatrix(aspectRatio,fov,nearClip,farClip));
		//vertColMat->SetMat4Uniform("_cameraMatrix", editorCamera.GetCameraMatrix());

		Mat4x4 perspMatrix = GetPerspectiveMatrix(aspectRatio,fov,nearClip,farClip);
		Mat4x4 camMatrix = editorCamera.GetCameraMatrix();

		glUniformMatrix4fv(glGetUniformLocation(vertColMat->shaderProgram,  "_perspMatrix"), 1, GL_TRUE, &perspMatrix.m[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(vertColMat->shaderProgram,  "_cameraMatrix"), 1, GL_TRUE,  &camMatrix.m[0][0]);

		Vector3 rightVector   = globalManipulator ?  X_AXIS : selectedObj->transform.Right();
		Vector3 upVector      = globalManipulator ?  Y_AXIS : selectedObj->transform.Up();
		Vector3 forwardVector = globalManipulator ?  Z_AXIS : selectedObj->transform.Forward();

		glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 1, 1, 0.8f, 1);

		for(GameObject* obj : objects){
			if(obj->GetComponent<LightComponent>() != nullptr){
				glBegin(GL_LINES);
				{
					Vector3 origin = obj->transform.GlobalPosition();
					Vector3 to = origin + X_AXIS/2;
					glVertex3f(origin.x, origin.y, origin.z);
					glVertex3f(to.x, to.y, to.z);
				}
				glEnd();
			}
		}

		glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 1, 0, 0, 1);
		glBegin(GL_LINES);
		{
			Vector3 origin = selectedObj->transform.GlobalPosition();
			Vector3 to = origin + rightVector/2;
			glVertex3f(origin.x, origin.y, origin.z);
			glVertex3f(to.x, to.y, to.z);
		}
		glEnd();

		glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0, 1, 0, 1);
		glBegin(GL_LINES);
		{
			Vector3 origin = selectedObj->transform.GlobalPosition();
			Vector3 to = origin + upVector/2;
			glVertex3f(origin.x, origin.y, origin.z);
			glVertex3f(to.x, to.y, to.z);
		}
		glEnd();

		glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0, 0, 1, 1);
		glBegin(GL_LINES);
		{
			Vector3 origin = selectedObj->transform.GlobalPosition();
			Vector3 to = origin + forwardVector/2;
			glVertex3f(origin.x, origin.y, origin.z);
			glVertex3f(to.x, to.y, to.z);
		}
		glEnd();
	}

	for(GuiElement* elem : editorGui){
		elem->OnGui();
	}

	glEnable(GL_DEPTH);
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

EditorScene::~EditorScene(){
	for(auto iter = selectionSim.staticBoxBodies.begin(); iter != selectionSim.staticBoxBodies.end(); iter++){
		delete (*iter);
	}

	for(GuiElement* elem : editorGui){
		delete elem;
	}
}