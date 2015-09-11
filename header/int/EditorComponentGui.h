#ifndef EDITORCOMPONENTGUI_H
#define EDITORCOMPONENTGUI_H

#include "../ext/simple-xml.h"

struct GuiText; struct GuiTextField; struct Component;

struct EditorComponentGui{
	XMLElement serializedComponent;
	string cachedInputText;
	Component* liveComponent;
	GuiText* fieldLabel;
	GuiTextField* inputText;

	int commaCount;

	EditorComponentGui();

	void Load();
	void Save();
};

#endif