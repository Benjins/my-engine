#include "../header/int/ResourceManager.h"
#include "../header/int/Texture.h"
#include "../header/int/Material.h"
#include "../header/int/Model.h"


MaterialManager::MaterialManager(int matCount){
	matAlloc = matCount;
	texAlloc = 20;
	
	materials = new Material[matAlloc];
	textures = new Texture[texAlloc];
}

Material* MaterialManager::GetMaterialByName(string name){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == name){
			return &(materials[i]);
		}
	}

	return NULL;
}
int GetMaterialIdByName(string name);
Material* GetMaterialById(int id);

Material* MaterialManager::LoadMaterial(string matName, string shaderName, string textureName, string bumpMapName){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == ""){
			materials[i].Switch(shaderName, this, textureName, bumpMapName);
			materials[i].uvOffset = Vector2(0,0);
			materials[i].uvScale = Vector2(1,1);
			materials[i].matName = matName;
			if(materials[i].mainTexture != NULL){
				materials[i].mainTexture->fileName = textureName;
			}
			if(materials[i].bumpMap != NULL){
				materials[i].bumpMap->fileName = bumpMapName;
			}
			return &(materials[i]);
		}
	}

	return NULL;
}

Texture* MaterialManager::LoadTexture(const string& fileName, bool forceInstance /*= false*/){
	for(int i = 0; i < texAlloc; i++){
		if(!forceInstance && textures[i].fileName == fileName){
			return &textures[i];
		}
		else if(textures[i].fileName == ""){
			textures[i].fileName = fileName;
			textures[i].textureTarget = GL_TEXTURE_2D;
			textures[i].Load(GL_TEXTURE0);
			return &textures[i];
		}
	}

	return nullptr;
}

Texture* MaterialManager::GetTextureByFileName(const string& fileName){
	for(int i = 0; i < texAlloc; i++){
		if(textures[i].fileName == fileName){
			return &textures[i];
		}
	}

	return nullptr;
}

Material* MaterialManager::GetFreeMaterial(){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == ""){
			return &(materials[i]);
		}
	}

	return nullptr;
}

void MaterialManager::Clear(){
	if(materials != NULL){
		delete[] materials;
	}

	materials = new Material[matAlloc];
}

MaterialManager::~MaterialManager(){
	if(materials != NULL){
		delete[] materials;
	}

	if(textures != nullptr){
		delete[] textures;
	}
}
