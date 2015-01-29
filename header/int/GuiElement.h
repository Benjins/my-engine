#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "../header/ext/Vector2.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

using std::string;

struct Texture; struct MaterialManager; struct Scene;

enum struct GuiType{
	RawTexture,
	Text,
	Slider
};

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

	//virtual void OnAddedToScene(Scene* scn);

	void OnGui() const;

	virtual ~GuiElement();
};

struct GuiSlider : GuiElement{

};

struct GuiText : GuiElement{
	
};

void GuiSetSliderValue(GuiElement* elem, float value);

#endif