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
struct Vector4;
struct Mat4x4;
struct ResourceManager;

struct Material{
	string shaderName;
	string vshaderText;
	string fshaderText;
	string matName;

	bool sharedMaterial;
	
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

	void Switch(string _shaderName, string textureName = "");
	
	void SetFloatUniform(string name, float value);
	void SetVec2Uniform(string name, Vector2 value);
	void SetVec3Uniform(string name, Vector3 value);
	void SetVec4Uniform(string name, Vector4 value);
	void SetMat4Uniform(string name, Mat4x4 value);
	void SetTextureUniform(string name, Texture* value);

	void SetMainTexture(Texture* value);

	void SetFloatUniform(int index, float value);
	void SetVec2Uniform(int index, Vector2 value);
	void SetVec3Uniform(int index, Vector3 value);
	void SetVec4Uniform(int index, Vector4 value);
	void SetMat4Uniform(int index, Mat4x4 value);
	void SetTextureUniform(int index, Texture* value);
	

	GLuint GetUniformByName(string name);
	int GetUniformIndexByName(string name);
	GLuint GetUniformByIndex(int index);

	void Release(ResourceManager* manager);

	~Material();
};

bool ReadFile(string fileName, string& readInto);

string TrimWhitespace(string param);

vector<string> GetUniformNames(string shaderCode);

#endif