#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include <fstream>
#include <iostream>
#include <cstring>

using std::ifstream; using std::cout; using std::cerr; using std::strlen;

Material::Material(void){
	mainTexture = NULL;
}

Material::Material(string _shaderName, string textureName){
	shaderProgram = glCreateProgram();
	shaderName = _shaderName;
	
	vshaderText = fshaderText = ""; 
	
	if(ReadFile(_shaderName + ".vs", vshaderText) 
	&& ReadFile(_shaderName + ".fs", fshaderText)){
		cout << "Added shader programs.\n";
		AddShader(shaderProgram, vshaderText.c_str(), GL_VERTEX_SHADER);
		AddShader(shaderProgram, fshaderText.c_str(), GL_FRAGMENT_SHADER);
		
		glLinkProgram(shaderProgram);
		glValidateProgram(shaderProgram);
		
		if(textureName != ""){
			cout << "mainTexture made.\n";
			mainTexture = new Texture(GL_TEXTURE_2D, textureName);
			mainTexture->Load();
			textureUniform = glGetUniformLocation(shaderProgram, "_mainTex");
			cameraUniform = glGetUniformLocation(shaderProgram, "_cameraMatrix");
			objectMatrixUniform = glGetUniformLocation(shaderProgram, "objectMatrix");
			//cout << (textureUniform == 0xffffffff ? "TexUniform is null\n" : "Texuniform is not null, all clear.\n");
		}
		else{
			mainTexture = NULL;
		}
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
			getline(fileIn, line);
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

Material::~Material(){
	if(mainTexture != NULL){
		delete mainTexture;
	}
}
