#ifndef LIGHT_H
#define LIGHT_H

struct Light{
	Vector3 position;
	Quaternion rotation;
	float intensity;
	bool isDirectional;
};


#endif