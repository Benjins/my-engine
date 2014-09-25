#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <GL/glew.h>

using std::string;

struct Texture;

struct Material{
	string shaderName;
	string vshaderText;
	string fshaderText;
	
	GLuint shaderProgram;
	Texture* mainTexture;
	
	GLuint textureUniform;
	
	Material(string _shaderName);
	
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
};

bool ReadFile(string fileName, string& readInto);

#endif