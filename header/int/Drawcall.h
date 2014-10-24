#ifndef DRAWCALL_H
#define DRAWCALL_H

#include "Model.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

struct Material;

struct DrawCall{
	//Data for OpenGL
	GLuint vertices;
	GLuint colors;
	GLuint uvs;
	
	int vertCount;
	
	Material* material;
	
	DrawCall(const Model& model, Material* mat);
	
	void Draw() const;
};

//Get uniform locations, and use them


#endif