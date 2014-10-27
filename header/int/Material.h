#ifndef MATERIAL_H
#define MATERIAL_H

#define MAX_NUMBER_OF_UNIFORMS 12

#include <string>
#include "Model.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

using std::string;

struct Texture;

struct Material{
	string shaderName;
	string vshaderText;
	string fshaderText;
	
	GLuint shaderProgram;
	Texture* mainTexture;

	GLuint uniforms[MAX_NUMBER_OF_UNIFORMS];
	vector<string> uniformNames;

	
	GLuint textureUniform;
	GLuint cameraUniform;
	GLuint objectMatrixUniform;
	
	Material(void);

	Material(string _shaderName, string textureName = "");
	
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

	GLuint GetUniformByName(string name);
	int GetUniformIndexByName(string name);
	GLuint GetUniformByIndex(int index);

	~Material();
};

bool ReadFile(string fileName, string& readInto);

string TrimWhitespace(string param);

vector<string> GetUniformNames(string shaderCode);

#endif