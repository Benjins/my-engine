#include "../header/int/ResourceManager.h"
#include "../header/int/Texture.h"
#include "../header/int/Material.h"
#include "../header/int/Model.h"


MaterialManager::MaterialManager(int matCount){
	matAlloc = matCount;
	
	materials = new Material[matCount];
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
			materials[i].Switch(shaderName, textureName, bumpMapName);
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
}
