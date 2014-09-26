#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Mat4.h"

#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){
	Mat4x4 matrix;

	Vector3 vec(1,2,3);

	Vector3 transVec = matrix * vec;

	cout << "X: " << transVec.x << "  Y: " << transVec.y << "  Z: " << transVec.z << endl;

	Scene x = Scene::getInstance(argc, argv);
	
	return 0;
}