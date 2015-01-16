#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Model.h"
#include "../header/int/Material.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"

#ifdef TESTING
#if defined(_WIN32) || defined(_WIN64)
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#  include <assert.h>
#endif
#endif

GameObject::GameObject(){
	scene = NULL;
	mesh = NULL;
	material = NULL;
	transform.gameObject = this;
}

GameObject::GameObject(Scene& sceneRef){
	scene = &sceneRef;
}

void GameObject::AddMesh(string fileName){
	if(mesh != NULL){
		delete[] mesh;
	}
	mesh = new Model(fileName);
}

void GameObject::AddMaterial(string matName, string textureName){
	material = scene->resources.LoadMaterial(matName + textureName, matName, textureName);
}

void GameObject::AddNamedMaterial(string matName){
	material = scene->resources.GetMaterialByName(matName);
}

void GameObject::OnUpdate(){
	for(auto iter = components.begin(); iter != components.end(); iter++){
		(*iter)->OnUpdate();
	}
}

void GameObject::OnCollision(Collider* col){
	//cout << "Gameobject::OnCollision(), on: " << name << ", with: " << col->gameObject->name << endl;

	for(auto iter = components.begin(); iter != components.end(); iter++){
		(*iter)->OnCollision(col);
	}

	if(transform.GetParent() != NULL && transform.GetParent()->gameObject != NULL){
		transform.GetParent()->gameObject->OnCollision(col);
	}
}


int GameObject::GLVertexCount() const{
	if(mesh == NULL){
		return 0;
	}
	else{
		return mesh->GLVertexCount();
	}
}

GameObject::~GameObject(){
	for(auto iter = components.begin(); iter != components.end(); iter++){
		Component* comp = *iter;
		delete comp;
	}

	if(mesh != NULL){
		delete mesh;
	}
	
	//The material is managed by MaterialManager, so we don't delete it.

}