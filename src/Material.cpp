#include "../header/int/Material.h"
#include <fstream>
#include <iostream>

using std::ifstream; using std::cout; using std::cerr;

Material::Material(){
}

Material::Material(string _shaderName){
	shaderProgram = glCreateProgram();
	shaderName = _shaderName;
	
	vShaderText = fShaderText = ""; 
	
	if(ReadFile(_shaderName + ".vs", vShaderText) 
	&& ReadFile(_shaderName + ".fs", fShaderText)){
		AddShader(shaderProgram, vShaderText.c_str(), GL_VERTEX_SHADER);
		AddShader(shaderProgram, fShaderText.c_str(), GL_FRAGMENT_SHADER);
		
		glLinkProgram(shaderProgram);
		glValidateProgram(shaderProgram);
		
		textureUniform = glGetUniformLocation(shaderProgram, "_mainTex");
	}
	else{
		//Probably should do something here...
	}
	
}

void Material::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType){
    GLuint ShaderObj = glCreateShader(ShaderType);

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
	
    glAttachShader(ShaderProgram, ShaderObj);
}

bool ReadFile(string fileName, string& readInto){
	ifstream fileIn(fileName.c_str());
	
	if (fileIn.is_open()) {
        string line;
        while(!fileIn.eof()) {
			getline(fileIn, line)
            readInto.append(line);
            readInto.append("\n");
        }
        
        fileIn.close();
        
        return true;
    }
    else {
        cerr << fileName << ": unable to open file.\n";
		return false;
    }
}