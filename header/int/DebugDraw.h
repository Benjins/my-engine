#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#pragma once

#include <vector>
#include "../../3dbasics/Vector3.h"

using std::vector;


struct DebugCube{
	Vector3 center;
	Vector3 forward;
	Vector3 right;
	Vector3 up;
	bool isWire;
}; 


struct DebugLine{
	Vector3 from;
	Vector3 to;
}; 


struct DebugSphere{
	Vector3 center;
	float size;
	bool isWire;
};

struct MaterialManager; struct Material;

struct DebugDraw{
	vector<DebugCube> debugCubes;
	vector<DebugLine> debugLines;
	vector<DebugSphere> debugSpheres;

	Material* mat;

	void Render(MaterialManager* resources);

	void Cube(const Vector3& position, const Vector3& size, bool isWire=true);
	void Cube(const Vector3& position, const Vector3& up, const Vector3& right, const Vector3& forward, bool isWire=true);
	void Line(const Vector3& from, const Vector3& to);
	void Sphere(const Vector3& center, float radius, bool isWire=true);
};


#endif
