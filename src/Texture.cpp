#include "../header/inc/Texture.h"
#include "../header/ext/EasyBMP.h"

Texture::Texture(GLenum TextureTarget, const std::string& _fileName){
    textureTarget = TextureTarget;
    fileName      = _fileName;
}

bool Texture::Load(){
    BMP image;
	
	image.ReadFromFile(fileName.c_str());
	int width = image.TellWidth();
	int height = image.TellHeight();
	
	RGBAPixel pixelData[width][height];
	
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

    return true;
}

void Texture::Bind(GLenum TextureUnit){
    glActiveTexture(TextureUnit);
    glBindTexture(textureTarget, textureObj);
}
