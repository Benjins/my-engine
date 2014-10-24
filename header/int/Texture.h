#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

using std::string;

struct RGBApixel;

struct Texture{
	//mip maps
	//bilinear
	//love me
	
	string fileName;
	GLenum textureTarget;
    GLuint textureObj;

	RGBApixel* pixelData;
	int width;
	int height;
	
	Texture(GLenum TextureTarget, const string& FileName);

    void Load(void);

    void Bind(GLenum TextureUnit);

	~Texture();
};


#endif