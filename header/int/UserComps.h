#ifndef USERCOMPS_H
#define USERCOMPS_H
#include "Component.h"
#include "Collider.h"
#include "RaycastHit.h"
#include "PhysicsSim.h"
#include "GameObject.h"
#include "Vector4.h"
#include "Scene.h"
#include "../ext/simple-xml.h"
#include <iostream>

using std::cout; using std::endl;

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

	virtual XMLElement Serialize(){
		XMLElement elem;
		elem.name = "FireGun";
		return elem;
	}

	virtual void Deserialize(const XMLElement& elem){
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

	virtual XMLElement Serialize(){
		XMLElement elem;
		elem.name = "MatChangeOnHit";
		return elem;
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

struct EnemyComp : HitComponent{
	Vector3 currentTarget;
	float speed;
	PhysicsSim* physics;
	GameObject* player;

	int health;

	EnemyComp(){
		speed = 2;
		health = 5;
	}

	virtual XMLElement Serialize(){
		XMLElement elem;
		elem.name = "EnemyComp";
		elem.attributes.push_back(XMLAttribute("speed",  to_string(speed)));
		elem.attributes.push_back(XMLAttribute("health", to_string(health)));

		return elem;
	}

	virtual void Deserialize(const XMLElement& elem){
		for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
			if(iter->name == "speed"){
				speed = atoi(iter->data.c_str());
			}
			else if(iter->name == "health"){
				health = atoi(iter->data.c_str());
			}
		}
	}

	void ResetTarget(){
		float x = ((float)(rand() % 500 - 250)) / 25;
		float z = ((float)(rand() % 500 - 250)) / 25;
		currentTarget = Vector3(x, 0, z);
	}

	virtual void OnHit(RaycastHit hitInfo, GameObject* sender){
		health--;
		float ratio = ((float)health)/5;
		gameObject->material->SetVec4Uniform("_color", Vector4(1.0, ratio, ratio, 1.0));
		if(health <= 0){
			gameObject->scene->RemoveObject(gameObject);	
		}
	}

	virtual void OnAwake(){
		ResetTarget();
		physics = gameObject->scene->physicsSim;
		player = gameObject->scene->FindGameObject("mainCam");
	}

	virtual void OnUpdate(){
		Vector3 toPlayer = player->transform.position - gameObject->transform.position;
		RaycastHit hitPlayer = physics->Raycast(gameObject->transform.position, toPlayer);
		
		Vector3 moveVec;

		if(hitPlayer.hit && hitPlayer.col->gameObject == player){
			moveVec = toPlayer;
		}
		else{
			moveVec = currentTarget - gameObject->transform.position;
		}
		
		moveVec.y = 0;

		if(moveVec.MagnitudeSquared() < 0.1f){
			ResetTarget();
		}
		else{
			moveVec = moveVec.Normalized() * gameObject->scene->deltaTime * speed;

			RaycastHit testHit = physics->Raycast(gameObject->transform.GlobalPosition(), moveVec);
			if(!testHit.hit || testHit.depth > moveVec.Magnitude() + 0.2f){
				gameObject->transform.position = gameObject->transform.position + moveVec;
			}
			else if (testHit.hit){
				Vector3 badVec = testHit.normal * DotProduct(moveVec, testHit.normal);
				Vector3 goodVec = moveVec - badVec;
				gameObject->transform.position = gameObject->transform.position + goodVec;
			}
		}

		float floorHeight = -10;
		RaycastHit lookDown = physics->Raycast(gameObject->transform.position, Y_AXIS*-1);
		if(lookDown.hit){
			floorHeight = lookDown.worldPos.y;
		}

		if(gameObject->transform.position.y <= floorHeight + 0.6f){
			gameObject->transform.position.y = floorHeight + 0.6f;
		}
		else{
			gameObject->transform.position.y -= 3 * gameObject->scene->deltaTime;
		}
	}
};

struct CameraControl : Component{
	Input* input;
	SC_Transform* camera;
	PhysicsSim* physics;
	GuiElement* slider;
	GuiText* healthBar;
	float speed;
	float velocity;

	bool isGrounded;
	float health;

	int prevX;
	int prevY;
	float xRot;
	float yRot;

	CameraControl(){
		speed = 5;
		prevX = 0;
		prevY = 0;
		xRot = 0;
		yRot = 0;
		velocity = 0;
		isGrounded = false;
		health = 1;
	}

	virtual XMLElement Serialize(){
		XMLElement elem;
		elem.name = "CameraControl";
		elem.attributes.push_back(XMLAttribute("speed",  to_string(speed)));
		elem.attributes.push_back(XMLAttribute("velocity", to_string(velocity)));
		elem.attributes.push_back(XMLAttribute("isGrounded", isGrounded ? "T" : "F"));
		elem.attributes.push_back(XMLAttribute("health", to_string(health)));

		return elem;
	}

	virtual void Deserialize(const XMLElement& elem){
		for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
			if(iter->name == "speed"){
				speed = atoi(iter->data.c_str());
			}
			else if(iter->name == "health"){
				health = atof(iter->data.c_str());
			}
			else if(iter->name == "velocity"){
				velocity = atof(iter->data.c_str());
			}
			else if(iter->name == "isGrounded"){
				isGrounded = (iter->data == "T");
			}
		}
	}

	virtual void OnAwake(){
		input = &gameObject->scene->input;
		camera = gameObject->scene->camera;
		physics = gameObject->scene->physicsSim;
		slider = gameObject->scene->guiElements[0];
		healthBar = static_cast<GuiText*>(gameObject->scene->FindGUIElement("healthText"));
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

		if(input->GetKeyUp('b')){
			GameObject* newEnemy = new GameObject();
			newEnemy->name = "enemyClone";
			newEnemy->scene = gameObject->scene;
			newEnemy->AddMesh("test.obj");
			newEnemy->AddMaterial("shader","Texture.bmp");
			newEnemy->AddComponent<BoxCollider>();
			newEnemy->AddComponent<EnemyComp>();
			newEnemy->transform.position = Vector3(-2, 1, 5);
			newEnemy->transform.scale = Vector3(0.3f, 1, 0.3f);
			gameObject->scene->AddObject(newEnemy);
		}

		health += (camera->GetParent()->position.y <= 1 ? -0.5f : 0.06f) * gameObject->scene->deltaTime;
		health = max(0.0f, min(1.0f, health));
		GuiSetSliderValue(slider, health);
		if(healthBar != nullptr){
			healthBar->text = "Health: " + to_string((int)(health*100));
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
		if(camera->GetParent()->position.y <= floorHeight + 0.4f){
			camera->GetParent()->position.y = floorHeight + 0.4f;
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

#endif