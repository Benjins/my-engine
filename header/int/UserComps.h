#ifndef USERCOMPS_H
#define USERCOMPS_H

#pragma once

#include "Component.h"
#include "RigidBody.h"
#include "Collider.h"
#include "RaycastHit.h"
#include "PhysicsSim.h"
#include "GameObject.h"
#include "Material.h"
#include "Vector4.h"
#include "Light.h"
#include "Scene.h"
#include "LoadingUtilities.h"
#include "Animation.h"
#include "Armature.h"
#include "AudioSystem.h"
#include "../ext/simple-xml.h"
#include "3DUtilities.h"
#include <iostream>
#include <assert.h>
#include "Macros.h"

using std::cout; using std::endl;

struct ParticleComponent : Component{
	
	virtual void OnAwake(){
		ParticleSystem sys;
		sys.maxLifetime = 15.4f;
		sys.maxParticleCount = 10000;
		sys.particlesPerSec = 30.0f;
		sys.startScale = 0.3f;
		sys.endScale = 0.1f;
		sys.gravityFactor = 0.2f;
		sys.spawnAngle = 90;
		sys.startVelocity = 2.2f;

		CollisionPlane plane1 = {Vector3(0.0f, 0.1f, 0.0f), Vector3(0,1.0f,0)};
		CollisionPlane plane2 = {Vector3(0.0f, 1.6f, 0.4f), Vector3(0,0.6f,0.8f)};
		sys.AddCollisionPlane(plane1);
		sys.AddCollisionPlane(plane2);

		sys.startCol = Vector4(1.0f, 0.9f, 0.1f, 1.0f);
		sys.endCol   = Vector4(0.8f, 0.2f, 0.1f, 0.0f);

		gameObject->scene->particles.push_back(sys);
		gameObject->scene->particles.back().Start();
	}
};

struct DoorComponent : Component{
	bool isLocked;
	bool isOpen;

	GameObject* player;

	DoorComponent(){
		isLocked = false;
		isOpen = false;
	}

	virtual XMLElement Serialize();

	virtual void Deserialize(const XMLElement& elem);

	virtual Component* Clone();

	virtual void OnAwake() override{
		player = gameObject->scene->FindGameObject("mainCam");
	}

	virtual void OnUpdate() override{
		if(gameObject->scene->input.GetKeyUp('t')){
			isLocked = !isLocked;
		}

		if(!isLocked && (gameObject->transform.GlobalPosition() - player->transform.GlobalPosition()).MagnitudeSquared() < 4){
			isOpen = true;
		}
		else{
			isOpen = false;
		}

		if(isOpen){
			gameObject->transform.scale.x = 0.1f;
		}
		else{
			gameObject->transform.scale.x = 1.0f;
		}
	}
};

struct IKAnimTestComp : Component{

	Armature* arm;

	bool doIk;

	virtual void OnAwake(){
		arm = nullptr;
		doIk = false;
	}

	virtual void OnUpdate() override{
		if(arm == nullptr){
			arm = gameObject->scene->FindGameObject("test2-obj")->mesh->armature;
			IKConstraint ik;
			BoneTransform* handR = arm->GetBoneByName("hand_R");
			ik.boneIndex = (handR - arm->bones);
			ik.constraintLength = 2;
			arm->ikConstraints.push_back(ik);
		}

		arm->ikConstraints[0].position = gameObject->transform.GlobalPosition();
	}

	virtual void OnEditorUpdate() override{
		OnUpdate();
	
		if(gameObject->scene->input.GetKeyUp(']')){
			doIk = !doIk;
		}

		if(doIk){
			arm->IKPass();
		}
	}

	virtual XMLElement Serialize();
	virtual Component* Clone();
};

struct PlayerComponent : Component{
	//virtual void OnCollision(Collider* col) override{
		
	//}
};

struct AnimationControlTest : Component{
	Armature* armature;
	float timeOut;
	Timer timer;
	bool isIdle;
	Input* input;

	virtual void OnAwake(){
		armature = gameObject->mesh->armature;
		timeOut = 3.5f;
		timer.Reset();
		isIdle = true;
		armature->BlendTo("idle", 0.0f);
		input = &gameObject->scene->input;
	}

	virtual void OnUpdate(){
		if(armature != nullptr){
			if(input->GetKeyUp('m')){
				timer.Reset();
				isIdle = false;
				armature->BlendTo("atk", 3.2f);
			}

			if(!isIdle && timer.GetTimeSince() >= timeOut){
				armature->BlendTo("idle", 3.2f);
				timer.Reset();
				isIdle = true;
			}
		}
	}

	virtual XMLElement Serialize();

	virtual Component* Clone();
};

struct AudioComponent : Component{
	string clipName;

	ALuint source;

	float volume;
	bool loop;
	bool autoPlay;

	AudioComponent(){
		volume = 1;
		loop = false;
		autoPlay = true;
	}

	virtual void OnAwake(){
		AudioClip* clip = gameObject->scene->audio.FindClip(clipName);
		assert(clip != nullptr);
		alGenSources(1, &source);

		alSourcei(source, AL_BUFFER, clip->buffer);
	  	alSourcef(source, AL_PITCH, 1);
	  	alSourcef(source, AL_GAIN,  volume);

	  	alSourcei(source,  AL_LOOPING, (loop ? AL_TRUE : AL_FALSE));
	  	if(autoPlay){
	  		alSourcePlay(source);
	  	}
	}

	virtual void OnUpdate(){
		gameObject->scene->debugDraw.Cube(gameObject->transform.GlobalPosition(), Vector3(2,2,2));

		Vector3 place = gameObject->transform.GlobalPosition();
		ALfloat placeData[] = {place.x, place.y, place.z};
		alSourcefv(source, AL_POSITION, placeData);
	}

	void Stop(){
		alSourceStop(source);
	}

	void Pause(){
		alSourcePause(source);
	}

	void Play(){
		alSourcePlay(source);
	}

	bool IsPlaying(){
		ALint state;
	    alGetSourcei(source, AL_SOURCE_STATE, &state);
	    return (state == AL_PLAYING);
	}

	void SetVolume(float volume){
		alSourcef(source, AL_GAIN,  volume);
	}

	virtual XMLElement Serialize();

	virtual void Deserialize(const XMLElement& elem);

	virtual Component* Clone();

	virtual ~AudioComponent(){
		alDeleteSources(1, &source);
	}
};

struct CameraControl : Component{
	Input* input;
	SC_Transform* camera;
	PhysicsSim* physics;
	GuiElement* slider;
	GuiText* healthBar;
	AudioComponent* audioComp;

	float speed;
	float velocity;

	bool isGrounded;
	float health;

	float stepDelay;
	float timeMoving;

	float characterHeight;
	float groundedAdjustment;

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
		timeMoving = 0;
		stepDelay = 0.6f;
		characterHeight = 0.4f;
		groundedAdjustment = 0.05f;
	}

	virtual XMLElement Serialize();

	virtual Component* Clone();

	virtual void Deserialize(const XMLElement& elem);

	virtual void OnAwake(){
		input = &gameObject->scene->input;
		camera = gameObject->scene->camera;
		physics = gameObject->scene->physicsSim;
		slider = gameObject->scene->guiSystem.FindGUIElement("healthSlider");
		healthBar = static_cast<GuiText*>(gameObject->scene->guiSystem.FindGUIElement("healthText"));
		audioComp = gameObject->GetComponent<AudioComponent>();
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
			isGrounded = false;
		}

		//health += (camera->GetParent()->position.y <= 1 ? -0.5f : 0.06f) * gameObject->scene->deltaTime;
		health = max(0.0f, min(1.0f, health));
		GuiSetSliderValue(slider, health);
		if(healthBar != nullptr){
			healthBar->text = "Health: " + to_string((int)(health*100));
		}

		moveVec.y = 0;
		if(moveVec.MagnitudeSquared() > 0){
			moveVec = moveVec.Normalized() * gameObject->scene->deltaTime * speed;

			RaycastHit testHit = physics->Raycast(camera->GlobalPosition(), moveVec);
			if(!testHit.hit || testHit.depth > moveVec.Magnitude() + 0.1f){
				camera->GetParent()->position = camera->GetParent()->position + moveVec;
			}
			else if (testHit.hit){
				Vector3 newNormal = testHit.normal;
				if(testHit.normal.y < 0){
					newNormal.y = 0;
					newNormal.Normalize();
				}
				float overlap = DotProduct(moveVec, newNormal);
				Vector3 badVec = newNormal * overlap;
				Vector3 goodVec = moveVec - badVec;

				if(goodVec.MagnitudeSquared() > 0){
					RaycastHit testHit2 = physics->Raycast(camera->GlobalPosition(), goodVec);
					if(!testHit2.hit || testHit2.depth > goodVec.Magnitude() + 0.1f){
						camera->GetParent()->position = camera->GetParent()->position + goodVec;
					}
				}
			}

			timeMoving += gameObject->scene->deltaTime;
		}
		else{
			timeMoving = 0;
		}

		float floorHeight = -10;
		RaycastHit lookDown = physics->Raycast(camera->GetParent()->position, Y_AXIS*-1);
		RaycastHit lookUp = physics->Raycast(camera->GetParent()->position, Y_AXIS);
		if(lookDown.hit){
			floorHeight = lookDown.worldPos.y;
		}

		if(lookUp.hit && lookUp.depth < 0.1f && velocity > 0){
			velocity = 0;
		}

		velocity -= gameObject->scene->deltaTime * 5;
		camera->GetParent()->position.y += velocity * gameObject->scene->deltaTime;
		if(camera->GetParent()->position.y <= floorHeight + (isGrounded ? characterHeight + groundedAdjustment : characterHeight)){
			camera->GetParent()->position.y = floorHeight + characterHeight;
			velocity = 0;
			isGrounded = true;
		}
		else{
			isGrounded = false;
		}

		if(timeMoving > 0.8f && isGrounded){
			audioComp->Play();
			timeMoving = 0;
		}
	}

	virtual ~CameraControl(){}
};

struct BulletComponent : Component{
	GameObject* player;
	float speed;
	float time;
	float timeLimit;

	BulletComponent(){
		speed = 3;
		timeLimit = 6;
		time = 0;
	}

	virtual void OnAwake(){
		player = gameObject->scene->FindGameObject("mainCam");
	}

	virtual XMLElement Serialize();
	virtual Component* Clone();

	virtual void Deserialize(const XMLElement& elem);

	virtual void OnCollision(Collider* col){ 
		if(col->gameObject == player){
			player->transform.children[0]->gameObject->GetComponent<CameraControl>()->health -= 0.1f;
		}
		else{
			//cout << "Collision with non-player: '" << col->gameObject->name << "'\n";
		}

		gameObject->scene->RemoveObject(gameObject);
	}

	virtual void OnUpdate(){
		gameObject->transform.position = gameObject->transform.position + gameObject->transform.Forward() * gameObject->scene->deltaTime * speed;

		time += gameObject->scene->deltaTime;
		if(time >= timeLimit){
			gameObject->scene->RemoveObject(gameObject);
		}
	}
};

struct PathNodeComponent : Component{
	
	DontSerialize int nodeId;

	virtual void OnAwake(){
		nodeId = gameObject->scene->pathfinding.AddNode(gameObject->transform.GlobalPosition());
	}

	virtual XMLElement Serialize();
	virtual Component* Clone();

	virtual void OnEditorUpdate(){
		for(PathNode& node : gameObject->scene->pathfinding.nodes){
			if(node.id == nodeId){
				node.position = gameObject->transform.GlobalPosition();
				break;
			}
		}
	}

	virtual ~PathNodeComponent(){
		int index = 0;
		for(PathNode& node : gameObject->scene->pathfinding.nodes){
			if(node.id == nodeId){
				assert(index < gameObject->scene->pathfinding.nodes.size());
				gameObject->scene->pathfinding.nodes.erase(gameObject->scene->pathfinding.nodes.begin() + index);
				break;
			}
			index++;
		}
	}
};

struct AnimationComponent : Component{
	//Tried putting these in a union, caused issues with destruction...
	Animation<float> floatAnim;
	Animation<Vector2> vec2Anim;
	Animation<Vector3> vec3Anim;
	Animation<Quaternion> quatAnim;

	AnimationType animType;
	AnimationTarget animTarget;
	float currentTime;

	bool loop;
	bool playAutomatically;
	bool isPlaying;

	virtual Component* Clone(){
		AnimationComponent* newAnim = new AnimationComponent();
		newAnim->animType = animType;
		newAnim->animTarget = animTarget;

		if(animType == AnimationType::Float){
			newAnim->floatAnim = floatAnim;
		}
		else if(animType == AnimationType::Vector2){
			newAnim->vec2Anim = vec2Anim;
		}
		else if(animType == AnimationType::Vector3){
			newAnim->vec3Anim = vec3Anim;
		}
		else if(animType == AnimationType::Quaternion){
			newAnim->quatAnim = quatAnim;
		}

		newAnim->loop = loop;
		newAnim->playAutomatically = playAutomatically;

		return newAnim;
	}

	virtual XMLElement Serialize(){
		XMLElement elem;
		elem.name = "AnimationComponent";
		elem.AddAttribute("type", EncodeAnimationType(animType));
		elem.AddAttribute("target", EncodeAnimationTarget(animTarget));
		elem.AddAttribute("loop", (loop ? "T" : "F"));
		elem.AddAttribute("autoplay", (playAutomatically ? "T" : "F"));

		if(animType == AnimationType::Float){
			for(const KeyFrame<float>& frame : floatAnim.keyFrames){
				XMLElement child;
				child.name = "KeyFrame";
				child.AddAttribute("time", to_string(frame.time));
				child.AddAttribute("value", to_string(frame.value));
				elem.children.push_back(child);
			}
		}
		else if(animType == AnimationType::Vector2){
			for(const KeyFrame<Vector2>& frame : vec2Anim.keyFrames){
				XMLElement child;
				child.name = "KeyFrame";
				child.AddAttribute("time", to_string(frame.time));
				child.AddAttribute("value", EncodeVector2(frame.value));
				elem.children.push_back(child);
			}
		}
		else if(animType == AnimationType::Vector3){
			for(const KeyFrame<Vector3>& frame : vec3Anim.keyFrames){
				XMLElement child;
				child.name = "KeyFrame";
				child.AddAttribute("time", to_string(frame.time));
				child.AddAttribute("value", EncodeVector3(frame.value));
				elem.children.push_back(child);
			}
		}
		else if(animType == AnimationType::Quaternion){
			for(const KeyFrame<Quaternion>& frame : quatAnim.keyFrames){
				XMLElement child;
				child.name = "KeyFrame";
				child.AddAttribute("time", to_string(frame.time));
				child.AddAttribute("value", EncodeQuaternion(frame.value));
				elem.children.push_back(child);
			}
		}

		return elem;
	}

	virtual void Deserialize(const XMLElement& elem){
		for(const XMLAttribute& attr : elem.attributes){
			if(attr.name == "type"){
				animType = ParseAnimationType(attr.data);
			}
			else if(attr.name == "target"){
				animTarget = ParseAnimationTarget(attr.data);
			}
			else if(attr.name == "loop"){
				loop = attr.data == "T";
			}
			else if(attr.name == "autoplay"){
				playAutomatically = attr.data == "T";
			}
		}

		//Maybe verify target and type?

		if(animType == AnimationType::Float){
			floatAnim.keyFrames.clear();
		}
		else if(animType == AnimationType::Vector2){
			vec2Anim.keyFrames.clear();
		}
		else if(animType == AnimationType::Vector3){
			vec3Anim.keyFrames.clear();
		}
		else if(animType == AnimationType::Quaternion){
			quatAnim.keyFrames.clear();
		}

		for(const XMLElement& child : elem.children){
			if(child.name == "KeyFrame"){
				float floatVal;
				Vector2 vec2Val;
				Vector3 vec3Val;
				Quaternion quatVal;
				float time;
				for(const XMLAttribute& attr : child.attributes){
					if(attr.name == "time"){
						time = atof(attr.data.c_str());
					}
					else if(attr.name == "value"){
						if(animType == AnimationType::Float){
							floatVal = atof(attr.data.c_str());
						}
						else if(animType == AnimationType::Vector2){
							vec2Val = ParseVector2(attr.data);
						}
						else if(animType == AnimationType::Vector3){
							vec3Val = ParseVector3(attr.data);
						}
						else if(animType == AnimationType::Quaternion){
							quatVal = ParseQuaternion(attr.data);
						}
					}
				}

				if(animType == AnimationType::Float){
					floatAnim.AddKeyFrame(floatVal, time);
				}
				else if(animType == AnimationType::Vector2){
					vec2Anim.AddKeyFrame(vec2Val, time);
				}
				else if(animType == AnimationType::Vector3){
					vec3Anim.AddKeyFrame(vec3Val, time);
				}
				else if(animType == AnimationType::Quaternion){
					quatAnim.AddKeyFrame(quatVal, time);
				}
			}
		}
	}

	virtual void OnAwake(){
		currentTime = 0;
		isPlaying = playAutomatically;
	}

	virtual void OnUpdate(){
		float length = 0;
		if(animType == AnimationType::Float){
			length = floatAnim.Length();
			floatAnim.loop = loop;
		}
		else if(animType == AnimationType::Vector2){
			length = vec2Anim.Length();
			vec2Anim.loop = loop;
		}
		else if(animType == AnimationType::Vector3){
			length = vec3Anim.Length();
			vec3Anim.loop = loop;
		}
		else if(animType == AnimationType::Quaternion){
			length = quatAnim.Length();
			quatAnim.loop = loop;
		}

		if(isPlaying){
			currentTime += gameObject->scene->deltaTime;
		}

		if(animTarget == AnimationTarget::Position){
			assert(animType == AnimationType::Vector3);
			gameObject->transform.position = vec3Anim.Evaluate(currentTime);
		}
		else if(animTarget == AnimationTarget::Scale){
			assert(animType == AnimationType::Vector3);
			gameObject->transform.scale = vec3Anim.Evaluate(currentTime);
		}
		else if(animTarget == AnimationTarget::UVOffset){
			assert(animType == AnimationType::Vector2);
			gameObject->material->uvOffset = vec2Anim.Evaluate(currentTime);
		}
		else if(animTarget == AnimationTarget::UVScale){
			assert(animType == AnimationType::Vector2);
			gameObject->material->uvScale = vec2Anim.Evaluate(currentTime);
		}
		else if(animTarget == AnimationTarget::Rotation){
			assert(animType == AnimationType::Quaternion);
			gameObject->transform.rotation = quatAnim.Evaluate(currentTime);
		}
	}
};

struct LightComponent : Component{
	float intensity;
	DontSerialize int id;
	bool isDirectional;

	virtual void OnAwake(){
		id = gameObject->scene->AddLight();
	}

	virtual void OnUpdate(){
		for(auto iter = gameObject->scene->lights.begin(); iter != gameObject->scene->lights.end(); iter++){
			if(iter->id == id){
				iter->intensity = intensity;
				iter->isDirectional = isDirectional;
				if(isDirectional){
					iter->direction = gameObject->transform.Forward();
				}
				else{
					iter->position = gameObject->transform.GlobalPosition();
				}
				break;
			}
		}
	}

	virtual void OnEditorUpdate(){
		OnUpdate();
	}

	virtual XMLElement Serialize();
	virtual Component* Clone();
	virtual void Deserialize(const XMLElement& elem);

	virtual ~LightComponent(){
		for(auto iter = gameObject->scene->lights.begin(); iter != gameObject->scene->lights.end(); iter++){
			if(iter->id == id){
				gameObject->scene->lights.erase(iter);
				break;
			}
		}
	}
};

struct TestComp : Component{
	virtual void OnCollision(Collider* col){
		cout << "Collision: " << col->gameObject->name << endl;
	}
};

struct SimpleAnimation : Component{
	Animation<Vector3> posAnim;
	float currTime;

	virtual void OnAwake(){
		currTime = 0;
	}
	
	virtual void OnUpdate(){
		currTime += gameObject->scene->deltaTime;
		gameObject->transform.position = posAnim.Evaluate(currTime);
	}
};

struct HitComponent : Component {

	virtual void OnHit(RaycastHit hitInfo, GameObject* sender) = 0;

	virtual ~HitComponent(){}
};

struct AnimateUVOffset : Component{

	Animation<Vector2> anim;
	float currentTime;

	virtual void OnAwake(){
		anim.AddKeyFrame(Vector2(0,0), 0);
		anim.AddKeyFrame(Vector2(5,5), 20);
		currentTime = 0;
	}

	virtual void OnUpdate(){
		currentTime += gameObject->scene->deltaTime;
		gameObject->material->uvOffset = anim.Evaluate(currentTime);
	}

	virtual ~AnimateUVOffset(){}
};

struct FireGun : Component{
	Input* input;
	PhysicsSim* physicsSim;
	SC_Transform* camera;

	GameObject* bulletPrefab;
	float bulletForce;

	FireGun(){
		bulletForce = 10000.0f;
	}

	virtual XMLElement Serialize();
	virtual Component* Clone();
	virtual void Deserialize(const XMLElement& elem);

	virtual void OnAwake(){
		input = &gameObject->scene->input;
		physicsSim = gameObject->scene->physicsSim;
		camera = gameObject->scene->camera;

		bulletPrefab = gameObject->scene->FindPrefab("bullet-rb");
	}

	

	virtual void OnUpdate(){
		if(input->GetMouseUp(GLUT_LEFT_BUTTON)){
			GameObject* bulletIntance = gameObject->scene->Instantiate(bulletPrefab, Vector3(0,0,0));
			bulletIntance->GetComponent<RigidBody>()->state.position = gameObject->transform.GlobalPosition() + gameObject->transform.Forward();
			bulletIntance->GetComponent<RigidBody>()->AddForce(gameObject->transform.Forward() * bulletForce);

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

	virtual XMLElement Serialize();

	virtual Component* Clone();

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
	DontSerialize Vector3 currentTarget;
	DontSerialize Vector3 longTermGoal;
	float speed;
	PhysicsSim* physics;
	Pathfinding* pathing;
	GameObject* player;
	vector<Vector3> path;

	GameObject* bulletPrefab;

	int health;
	int maxHealth;

	DontSerialize float reloadTimeCounter;
	float reloadTime;

	DontSerialize bool pathNeedsUpdate;

	EnemyComp(){
		speed = 2;
		health = 5;
		maxHealth = 5;
		pathNeedsUpdate = true;
		reloadTimeCounter = 0;
		reloadTime = 2 + ((float)(rand() % 500)) / 500;;
	}

	virtual XMLElement Serialize();

	virtual Component* Clone();

	virtual void Deserialize(const XMLElement& elem);

	void ResetTarget(){
		float x = ((float)(rand() % 500 - 250)) / 25;
		float z = ((float)(rand() % 500 - 250)) / 25;
		currentTarget = Vector3(x, 0, z);
	}

	virtual void OnHit(RaycastHit hitInfo, GameObject* sender){
		health--;
		float ratio = ((float)health)/maxHealth;
		cout << "Ratio: " << ratio << endl;
		//glUseProgram(gameObject->material->shaderProgram);
		//gameObject->material->SetVec4Uniform("_color", Vector4(1.0f - ratio, 0, 0, 1.0f));
		if(health <= 0){
			GameObject* newObj = gameObject->Clone();
			gameObject->material->matName = "";
			newObj->transform.position = gameObject->transform.position + Vector3(0.5f, 1, 0);
			gameObject->scene->AddObjectAndDescendants(newObj);
			gameObject->scene->RemoveObject(gameObject);	
		}
	}

	virtual void OnAwake(){
		ResetTarget();
		physics = gameObject->scene->physicsSim;
		pathing = &gameObject->scene->pathfinding;
		player = gameObject->scene->FindGameObject("mainCam");
		bulletPrefab = gameObject->scene->FindPrefab("bullet");
		gameObject->material->SetVec4Uniform("_color", Vector4(0, 0, 0, 1.0f));
	}

	virtual void OnUpdate(){
		reloadTimeCounter += gameObject->scene->deltaTime;

		Vector3 toPlayer = player->transform.position - gameObject->transform.position;
		RaycastHit hitPlayer = physics->Raycast(gameObject->transform.position, toPlayer);

		if(hitPlayer.hit && hitPlayer.col->gameObject == player){
			currentTarget = player->transform.position;
			longTermGoal = currentTarget;
			pathNeedsUpdate = true;
			gameObject->material->SetVec4Uniform("_color", Vector4(0, 1, 0, 1.0f));
		}
		else{
			//gameObject->material->SetVec4Uniform("_color", Vector4(0, 0, 0, 1.0f));
			if(pathNeedsUpdate){
				path = pathing->FindPath(gameObject->transform.position, longTermGoal);
				pathNeedsUpdate = false;
			}
			if(path.size() > 0)
			{
				currentTarget = path.back();
			}
		}
		
		Vector3 moveVec = currentTarget - gameObject->transform.position;

		moveVec.y = 0;

		if(moveVec.MagnitudeSquared() < 0.05f){
			if(path.size() > 0){
				path.pop_back();
				moveVec = moveVec.Normalized() * gameObject->scene->deltaTime * speed;
			}
			else{
				moveVec = Vector3(0,0,0);
			}
		}
		else{
			moveVec = moveVec.Normalized() * gameObject->scene->deltaTime * speed;
		}

		RaycastHit testHit = physics->Raycast(gameObject->transform.GlobalPosition(), moveVec);
		if (testHit.hit && testHit.depth < moveVec.Magnitude() + 0.2f){
			Vector3 badVec = testHit.normal * DotProduct(moveVec, testHit.normal);
			moveVec = moveVec - badVec;
		}

		float angle=0;
		if(moveVec.MagnitudeSquared() < 0.0001f){
			angle = 0;
		}
		else{
			angle = acos(moveVec.Normalized().z);
			if(moveVec.x < 0){
				angle = -angle;
			}

			Quaternion targetRot = Quaternion(Y_AXIS, angle);
			gameObject->transform.rotation = RotateTowards(gameObject->transform.rotation, targetRot, 60*gameObject->scene->deltaTime);
		}
		
		//cout << "angle: " << angle << "\n";
		
		gameObject->transform.position = gameObject->transform.position + gameObject->transform.Forward() * speed * gameObject->scene->deltaTime;

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

		if(reloadTimeCounter > reloadTime){
			reloadTimeCounter = 0;
			Vector3 spawnPos = gameObject->transform.position + gameObject->transform.Forward();
			GameObject* instance = gameObject->scene->Instantiate(bulletPrefab, spawnPos, gameObject->transform.rotation);
		}
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