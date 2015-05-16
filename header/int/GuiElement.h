#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "../header/ext/Vector2.h"
#include "../header/int/FontBMPMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

using std::string;

struct Texture; struct MaterialManager; struct Scene; struct FUV;

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

	virtual void OnGui() const;

	virtual ~GuiElement();
};

struct GuiSlider : GuiElement{

};

struct GuiText : GuiElement{
	string text;
	FUV fuv;
	GLuint texObj;

	GuiText(MaterialManager* resources, FUV& _fuv);

	virtual void OnGui() const;

	virtual ~GuiText();
};

void GuiSetSliderValue(GuiElement* elem, float value);

#endif