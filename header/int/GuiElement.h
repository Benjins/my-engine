#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "../header/ext/Vector2.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

using std::string;

struct Texture; struct MaterialManager;

struct GuiElement{
	
	Texture* tex;
	Vector2 position;
	Vector2 scale;
	int depth; //Higher depths drawn on top of lower depths
	GLint guiProgram;
	GLuint texLoc;
	string name;

	GuiElement();
	GuiElement(MaterialManager* resources);

	void OnGui() const;

	~GuiElement();
};

void GuiSetSliderValue(GuiElement* elem, float value);

#endif