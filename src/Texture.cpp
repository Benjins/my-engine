#include "../header/int/Texture.h"
#include "../header/ext/EasyBMP.h"

Texture::Texture(GLenum TextureTarget, const std::string& _fileName){
    textureTarget = TextureTarget;
    fileName      = _fileName;
	pixelData = NULL;
}

//Requires OpenGL context
void Texture::Load(void){
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
    glBindTexture(textureTarget, textureObj);
    glTexImage2D(textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Bind(GLenum TextureUnit){
    glActiveTexture(TextureUnit);
    glBindTexture(textureTarget, textureObj);
}


Texture::~Texture(){
	if(pixelData != NULL){
		delete[] pixelData;
	}
}