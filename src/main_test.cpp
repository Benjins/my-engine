#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"

#include <iostream>
using std::cout; using std::endl;

int main(){
	Scene x;
	
	GameObject y;
	
	x.AddObject(y);

	y.AddComponent<Component>();
	
	Component* z = y.GetComponent<Component>();
	
	return 0;
}