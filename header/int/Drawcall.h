#ifndef DRAWCALL_H
#define DRAWCALL_H

#include "Model.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

struct Material; struct GameObject;

struct DrawCall{
	//Data for OpenGL
	GLuint vertices;
	GLuint normals;
	GLuint tangents;
	GLuint colors;
	GLuint uvs;
	GLuint boneWeights;
	GLuint boneIndices;
	GLuint boneCount;
	
	int vertCount;
	
	//Material* material;
	GameObject* obj;
	
	DrawCall(GameObject* _obj);
	
	void Draw() const;
};

//Get uniform locations, and use them

void CalculateTangents(Model* model, vector<Vector3>& outTangentData);

#endif