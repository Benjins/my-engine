#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

using std::string;

struct Texture{
	//mip maps
	//bilinear
	//love me
	
	string fileName;
	GLenum textureTarget;
    GLuint textureObj;
	
	Texture(GLenum TextureTarget, const string& FileName);

    void Load();

    void Bind(GLenum TextureUnit);

private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
};


#endif