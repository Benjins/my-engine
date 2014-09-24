#include "../header/int/Scene.h"

Scene::Scene(){
	
}

GameObject& Scene::AddObject(GameObject& obj){
	objects.push_back(obj);
	return obj;
}

void Scene::OnUpdate(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		iter->OnUpdate();
	}
}