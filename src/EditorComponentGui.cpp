#include "../header/int/EditorComponentGui.h"
#include "../header/int/GuiElement.h"
#include "../header/int/Component.h"
#include "../header/int/Collider.h"

#include <algorithm>

EditorComponentGui::EditorComponentGui(){
	liveComponent = nullptr;
	fieldLabel = nullptr;
	inputText = nullptr;
}

void EditorComponentGui::Load(){
	serializedComponent = liveComponent->Serialize();
	for(const XMLAttribute& attr : serializedComponent.attributes){
		if(attr.name == fieldLabel->text){
			inputText->text = attr.data;
			cachedInputText = inputText->text;
			commaCount = std::count(cachedInputText.begin(), cachedInputText.end(), ',');
			break;
		}
	}	
}

void EditorComponentGui::Save(){
	if(cachedInputText != inputText->text){
		cachedInputText = inputText->text;
		int currentCommaCount = std::count(cachedInputText.begin(), cachedInputText.end(), ',');

		if(currentCommaCount == commaCount){
			serializedComponent = liveComponent->Serialize();
			for(auto iter = serializedComponent.attributes.begin(); iter !=  serializedComponent.attributes.end(); ++iter){
				if(iter->name == fieldLabel->text){
					serializedComponent.attributes.erase(iter);
					break;
				}
			}

			serializedComponent.AddAttribute(fieldLabel->text, inputText->text);
			liveComponent->Deserialize(serializedComponent);
		}
	}
}

