#ifndef LIGHT_H
#define LIGHT_H

struct Light{
	Vector3 position;
	Vector3 direction;
	float intensity;
	int id;
	bool isDirectional;
};


#endif