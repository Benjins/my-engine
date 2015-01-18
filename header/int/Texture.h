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
struct ResourceManager;

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
	
	Texture();
	Texture(int _width, int _height);
	Texture(GLenum TextureTarget, const string& FileName);

    void Load(void);

	void SetPixel(int x, int y, float r, float g, float b);
	void GetPixel(int x, int y);
	void Apply();

    void Bind(GLenum TextureUnit);

	void Release(ResourceManager* manager);

	~Texture();
};


#endif