#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include "GuiElement.h"

#include <vector>

using std::vector;

struct GuiSystem{
	vector<GuiElement*> elements;

	GuiElement* mouseDownElement;
	GuiElement* selectedElem;

	GuiSystem(){
		mouseDownElement = nullptr;
		selectedElem = nullptr;
	}

	void RenderGui();

	GuiElement* OnMouseDown(const Vector2& hitPoint);
	GuiElement* OnMouseDrag(const Vector2& hitPoint);
	GuiElement* OnMouseUp(const Vector2& hitPoint);

	~GuiSystem(){
		mouseDownElement = nullptr;

		for(GuiElement* elem : elements){
			delete elem;
		}

		elements.clear();
	}

};


#endif
