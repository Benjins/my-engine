#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Model.h"
#include "../header/int/Material.h"

#ifdef TESTING
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#  include <assert.h>
#endif

GameObject::GameObject(){
	mesh = NULL;
	material = NULL;
}

GameObject::GameObject(Scene& sceneRef){
	scene = &sceneRef;
}

void GameObject::AddMesh(string fileName){
	mesh = new Model(fileName);
}

void GameObject::AddMaterial(string matName, string textureName){
	material = new Material(matName, textureName);
}

void GameObject::OnUpdate(){
	for(auto iter = components.begin(); iter != components.end(); iter++){
		(*iter)->OnUpdate();
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

	if(material != NULL){
		delete material;
	}
}