#include "../header/int/Texture.h"
#include "../header/int/ResourceManager.h"
#include "../header/ext/EasyBMP.h"
#include <stdio.h>

Texture::Texture(){
	fileName = "";
	this->pixelData = NULL;
}

Texture::Texture(int _width, int _height){
	fileName = "";
	width = _width;
	height = _height;

	textureTarget = GL_TEXTURE_2D;
	pixelData = new RGBApixel[width*height];

	glGenTextures(1, &textureObj);
    Apply();
}

Texture::Texture(GLenum TextureTarget, const std::string& _fileName){
    textureTarget = TextureTarget;
    fileName      = _fileName;
	pixelData = NULL;
}

//Requires OpenGL context
void Texture::Load(GLenum TextureTarget){

	FILE* bmpFile = fopen(fileName.c_str());

	

	fclose(bmpFile);

    BMP image;
	
	image.ReadFromFile(fileName.c_str());
	width = image.TellWidth();
	height = image.TellHeight();

	pixelData = new RGBApixel[width*height];
	for(int j = 0; j < height; j++){
		for(int i = 0; i < width; i++){
			pixelData[j*width+i] = image.GetPixel(i,j); 
		}
	}

    glGenTextures(1, &textureObj);
    Apply(TextureTarget);
}

void Texture::SetPixel(int x, int y, float r, float g, float b){
	RGBApixel pix;
	pix.Red   = (unsigned char)(255*r);
	pix.Green = (unsigned char)(255*g);
	pix.Blue  = (unsigned char)(255*b);
	pix.Alpha = 255;

	int index = y * width + x;
	pixelData[index] = pix;
}

void Texture::SetPixel(int x, int y, RGBApixel col){
	int index = y * width + x;
	pixelData[index] = col;
}

void Texture::GetPixel(int x, int y){
	//Not implemented yet.
}

void Texture::Apply(GLenum TextureTarget){
	if(pixelData != NULL){

		glActiveTexture(TextureTarget);
		glBindTexture(textureTarget, textureObj);
		glTexImage2D(textureTarget, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);
		glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void Texture::Bind(GLenum TextureUnit){
    glActiveTexture(TextureUnit);
    glBindTexture(textureTarget, textureObj);
}

void Texture::Release(ResourceManager* manager){
	glDeleteTextures(1, &textureObj);
	if(pixelData != NULL){
		delete[] pixelData;
		pixelData = NULL;
	}

	fileName = "";
}

Texture::~Texture(){
	fileName="";
	if(pixelData != NULL){
		delete[] pixelData;
	}
}