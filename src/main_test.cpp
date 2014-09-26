#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"

#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){
	Scene x = Scene::getInstance(argc, argv);

	cout << "Here we go\n";
	
	GameObject y;
	
	y.AddMaterial("shader");
	y.AddMesh("test.obj");

	if(y.mesh != NULL && y.material != NULL){
		cout << "Proper mesh and material.\n";
	}

	x.AddObject(y);
	
	return 0;
}