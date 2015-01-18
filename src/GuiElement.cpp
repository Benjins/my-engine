#include "../header/int/GuiElement.h"
#include "../header/int/Texture.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Material.h"


GuiElement::GuiElement(){
	tex = NULL;
	guiProgram = 0;
	depth = 0;
	texLoc = 0;
	position = Vector2(0,0);
	scale = Vector2(0,0);
	name = "";
}

GuiElement::GuiElement(MaterialManager* resources){
	Material* mat = resources->GetMaterialByName("gui");
	guiProgram = mat->shaderProgram;
	texLoc = mat->GetUniformByName("_guiTex");

	name = "";
	tex = NULL;
	depth = 0;
	position = Vector2(0,0);
	scale = Vector2(0,0);
}

void GuiElement::OnGui() const{
	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	glUseProgram(guiProgram);
	if(tex != NULL){
		glBindTexture(tex->textureTarget, tex->textureObj);
	}
	glUniform1i(texLoc, 0);

	Vector2 corners[4] = { Vector2(position.x - scale.x/2, position.y - scale.y/2),
						   Vector2(position.x + scale.x/2, position.y - scale.y/2),
						   Vector2(position.x - scale.x/2, position.y + scale.y/2),
						   Vector2(position.x + scale.x/2, position.y + scale.y/2)};

	glBegin(GL_TRIANGLE_STRIP);
	for(int i = 0; i < 4; i++){
		Vector3 vert = Vector3(corners[i].x, corners[i].y, 0.5);
		vert = vert * 2 - Vector3(1,1,0);
		glVertex3f(vert.x, vert.y, vert.z);
	}
	glEnd();
	glUseProgram(currProgram);
}


void GuiSetSliderValue(GuiElement* elem, float value){
	Texture* tex = elem->tex;
	for(int i = 0; i < tex->width; i++){
		for(int j = 0; j < tex->height; j++){
			if(i < tex->width * value){
				tex->SetPixel(i,j,1,0.2,0.2);
			}
			else{
				tex->SetPixel(i,j,0.2,0.8,0.8);
			}
		}
	}

	tex->Apply();
}


GuiElement::~GuiElement(){
	if(tex != NULL){
		delete tex;
	}
}
