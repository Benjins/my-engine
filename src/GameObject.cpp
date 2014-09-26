#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Model.h"
#include "../header/int/Material.h"

GameObject::GameObject(){
}

GameObject::GameObject(Scene& sceneRef){
	scene = &sceneRef;
}

void GameObject::AddMesh(string fileName){
	cout << "AddMesh: " << fileName << endl;
	mesh = new Model(fileName);
}

void GameObject::AddMaterial(string matName, string textureName){
	cout << "AddMat: " << matName << endl;
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