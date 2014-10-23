#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include <fstream>
#include <iostream>
#include <cstring>

#ifdef TESTING
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#  include <assert.h>
#endif

using std::ifstream; using std::cout; using std::cerr; using std::strlen;

Material::Material(void){
	mainTexture = NULL;
}


//Requires an OpenGL context
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

string TrimWhitespace(string param){
	string newStr = param;

	int leadingCount = 0;

	for(auto iter = newStr.begin(); iter != newStr.end(); iter++){
		char single = *iter;
		if(single == ' ' || single == '\n' || single == '\t'){
			leadingCount++;
		}
		else{
			break;
		}
	}

	int trailingCount = 0;

	for(auto iter = newStr.rbegin(); iter != newStr.rend(); iter++){
		char single = *iter;
		if(single == ' ' || single == '\n' || single == '\t'){
			trailingCount++;
		}
		else{
			break;
		}
	}

	return newStr.substr(leadingCount, newStr.size() - leadingCount - trailingCount);
}

vector<string> GetUniformNames(string shaderCode){
	//TO-DO write this code.
	vector<string> lines = SplitStringByDelimiter(shaderCode, "\n");

	vector<string> uniformNames = vector<string>();

	for(auto iter = lines.begin(); iter != lines.end(); iter++){
		string trimmedLine = TrimWhitespace(*iter);
		if(trimmedLine.substr(0,7) == "uniform"){
			int lastSpace = trimmedLine.find_last_of(' ') + 1;
			int uniformLength = trimmedLine.size() - lastSpace - 1;
			uniformNames.push_back(trimmedLine.substr(lastSpace, uniformLength));
		}
	}

	return uniformNames;
}

Material::~Material(){
	if(mainTexture != NULL){
		delete mainTexture;
	}
}
