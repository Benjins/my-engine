#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "../ext/Vector2.h"
#include "FontBMPMaker.h"
#include "Texture.h"
#include "../ext/EasyBMP.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include <string>
#include <vector>

using std::string; using std::vector;

struct Texture; struct MaterialManager; struct Scene; struct FUV; struct XMLElement;

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

protected:
	GuiElement* parent;
public:
	vector<GuiElement*> children;

	bool isVisible;
	bool isPassive;

	GLint guiProgram;
	GLuint texLoc;
	string name;

	GuiElement();
	GuiElement(MaterialManager* resources);

	void SetParent(GuiElement* newParent);

	virtual void OnMouseDown(const Vector2& hitPoint){
		if(tex != nullptr){
			for(int i = 0; i < tex->width * tex->height; i++){
				tex->pixelData[i].Red *= 2;
			}

			tex->Apply();
		}
	}
	virtual void OnMouseUp(const Vector2& hitPoint){
		if(tex != nullptr){
			for(int i = 0; i < tex->width * tex->height; i++){
				tex->pixelData[i].Red /= 2;
			}

			tex->Apply();
		}
	}
	virtual void OnMouseDrag(const Vector2& hitPoint){}
	virtual void OnClicked(const Vector2& hitPoint){}

	bool WasHit(const Vector2& hitPoint){
		if(isPassive){
			return false;
		}

		return hitPoint.x >= (position.x - scale.x/2) && hitPoint.x <= (position.x + scale.x/2)
			&& hitPoint.y >= (position.y - scale.y/2) && hitPoint.y <= (position.y + scale.y/2);
	}

	GuiElement* GetParent() const { return parent; }

	virtual void OnGui() const;

	virtual XMLElement Serialize();

	virtual ~GuiElement();
};

struct GuiSlider : GuiElement{

};

struct GuiText : GuiElement{
	string text;
	FUV fuv;
	GLuint texObj;
	string fuvFileName;
	Vector2 textScale;

	GuiText(MaterialManager* resources, const string& _fuvFileName);

	virtual void OnGui() const;

	virtual XMLElement Serialize();

	virtual ~GuiText();
};

void GuiSetSliderValue(GuiElement* elem, float value);

#endif