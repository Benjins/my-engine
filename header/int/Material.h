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
	GLuint cameraUniform;
	GLuint objectMatrixUniform;
	
	Material(void);

	Material(string _shaderName, string textureName = "");
	
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

	~Material();
};

bool ReadFile(string fileName, string& readInto);

#endif