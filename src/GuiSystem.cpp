#include "../header/int/GuiSystem.h"
#include <iostream>

using std::cout; using std::endl;

void GuiSystem::RenderGui(){
	for(GuiElement* elem : elements){
		elem->OnGui();
	}
}

GuiElement* GuiSystem::OnMouseDown(const Vector2& hitPoint){
	for(GuiElement* elem : elements){
		if(elem->WasHit(hitPoint)){
			elem->OnMouseDown(hitPoint);
			mouseDownElement = elem;
			return elem;
		}
	}

	mouseDownElement = nullptr;

	return nullptr;
}

GuiElement* GuiSystem::OnMouseDrag(const Vector2& hitPoint){
	for(GuiElement* elem : elements){
		if(elem->WasHit(hitPoint)){
			elem->OnMouseDrag(hitPoint);
			return elem;
		}
	}

	return nullptr;
}

GuiElement* GuiSystem::OnMouseUp(const Vector2& hitPoint){
	if(mouseDownElement != nullptr){
		mouseDownElement->OnMouseUp(hitPoint);
	}

	for(GuiElement* elem : elements){
		if(elem != mouseDownElement && elem->WasHit(hitPoint)){
			//elem->OnMouseUp(hitPoint);
			mouseDownElement = nullptr;
			selectedElem = elem;
			return elem;
		}
	}

	selectedElem = nullptr;
	mouseDownElement = nullptr;
	return nullptr;
}