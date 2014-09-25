#include "../header/int/Texture.h"
#include "../header/ext/EasyBMP.h"

Texture::Texture(GLenum TextureTarget, const std::string& _fileName){
    textureTarget = TextureTarget;
    fileName      = _fileName;
	pixelData = NULL;
}

void Texture::Load(void){
    BMP image;
	
	image.ReadFromFile(fileName.c_str());
	width = image.TellWidth();
	height = image.TellHeight();

	pixelData = new RGBApixel*[width];
	for(int i = 0; i < width; i++){
		pixelData[i] = new RGBApixel[height];
	}
	
	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			pixelData[i][j] = image.GetPixel(i,j);
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
		for(int i = 0; i < width; i++){
			delete[] pixelData[i];
		}

		delete[] pixelData;
	}
}