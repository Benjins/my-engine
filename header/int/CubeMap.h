#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>
#include "Texture.h"

using std::string;

struct Mat4x4; struct MaterialManager; struct Material;

struct CubeMap{
	Material* material;

	GLuint cubeMapObj;

	GLuint cubeMeshObj;

	int glVertexCount;
	
	static GLenum types[];

	CubeMap();

	void Load(string fileNames[], const string& shaderName, MaterialManager* resources);

	void Bind(GLenum textureUnit);

	void Render(const Mat4x4& cameraMat, const Mat4x4& perspMat);

};


#endif