#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Mat4.h"
#include <iomanip>

#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){


	Scene x = Scene::getInstance(argc, argv);

	//x.Init();

	x.Start();
	
	return 0;
}