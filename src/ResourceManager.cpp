#include "../header/int/ResourceManager.h"
#include "../header/int/Texture.h"
#include "../header/int/Material.h"
#include "../header/int/Model.h"


ResourceManager::ResourceManager(int matCount, int textureCount, int meshCount ){
	materials = NULL;
	textures  = NULL;
	meshes    = NULL;

	matAlloc  = 0;
	texAlloc  = 0;
	meshAlloc = 0;

	materialRefs = NULL;
	textureRefs  = NULL;
	meshRefs     = NULL;

	ReserveMaterials(matCount);
	ReserveTextures(textureCount);
	ReserveMeshes(meshCount);
}

void ResourceManager::ReserveMaterials(int newMatCount){
	if(matAlloc < newMatCount){
		Material* newMats = new Material[newMatCount];
		if(materials != NULL){
			memcpy(newMats, materials, matAlloc*sizeof(Material));
			delete[] materials;
		}
		materials = newMats;

		unsigned int* newMatRefs = new unsigned int[newMatCount];
		if(materialRefs != NULL){
			memcpy(newMatRefs, materialRefs, matAlloc*sizeof(unsigned int));
			delete[] materialRefs;
			for(int i = matAlloc; i < newMatCount; i++){
				newMatRefs[i] = 0;
			}
		}
		for(int i = 0; i < newMatCount; i++){
			newMatRefs[i] = 0;
		}
		materialRefs = newMatRefs;

		materials = newMats;
		matAlloc = newMatCount;
	}
}
void ResourceManager::ReserveTextures(int newTexCount){
	if(texAlloc < newTexCount){
		Texture* newTex = new Texture[newTexCount];
		if(textures != NULL){
			memcpy(newTex, textures, texAlloc*sizeof(Texture));
			delete[] textures;
		}
		textures = newTex;

		unsigned int* newTexRefs = new unsigned int[newTexCount];
		if(textureRefs != NULL){
			memcpy(newTexRefs, textureRefs, texAlloc*sizeof(unsigned int));
			delete[] textureRefs;
			for(int i = texAlloc; i < newTexCount; i++){
				newTexRefs[i] = 0;
			}
		}
		for(int i = 0; i < newTexCount; i++){
			newTexRefs[i] = 0;
		}
		textureRefs = newTexRefs;

		texAlloc = newTexCount;
	}
}
void ResourceManager::ReserveMeshes(int newMeshCount){
	if(meshAlloc < newMeshCount){
		Model* newMesh = new Model[newMeshCount];
		if(meshes != NULL){
			memcpy(newMesh, meshes, meshAlloc*sizeof(Model));
			delete[] meshes;
		}
		meshes = newMesh;

		unsigned int* newMeshRefs = new unsigned int[newMeshCount];
		if(meshRefs != NULL){
			memcpy(newMeshRefs, meshRefs, meshAlloc*sizeof(unsigned int));
			delete[] meshRefs;
			for(int i = meshAlloc; i < newMeshCount; i++){
				newMeshRefs[i] = 0;
			}
		}
		for(int i = 0; i < newMeshCount; i++){
			newMeshRefs[i] = 0;
		}
		meshRefs = newMeshRefs;

		meshAlloc = newMeshCount;
	}
}

Material* ResourceManager::GetMaterialByName(string name){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == name){
			return &(materials[i]);
		}
	}

	return NULL;
}
int GetMaterialIdByName(string name);
Material* GetMaterialById(int id);

Texture* ResourceManager::GetTextureByName(string name){
	for(int i = 0; i < texAlloc; i++){
		if(textures[i].fileName == name){
			return &(textures[i]);
		}
	}

	return NULL;
}
int GetTextureIdByName(string name);
Texture* GetTextureById(int id);

Model* ResourceManager::GetMeshByName(string name){
	for(int i = 0; i < meshAlloc; i++){
		if(meshes[i].name == name){
			return &(meshes[i]);
		}
	}

	return NULL;
}
int GetMeshIdByName(string name);
Model* GetMeshById(int id);

Material* ResourceManager::LoadMaterial(string shaderName, string textureName){
	Material* mat = GetMaterialByName(shaderName + textureName) ;
	if(mat != NULL){
		int index = (mat - materials)/sizeof(Material);
		materialRefs[index]++;
		return mat;
	}

	for(int i = 0; i < matAlloc; i++){
		if(materialRefs[i] == 0){
			materials[i] = Material(shaderName, this, textureName);
			materialRefs[i] = 1;
			return &(materials[i]);
		}
	}

	return NULL;
}
Texture* ResourceManager::LoadTexture(string textureName){
	Texture* tex = GetTextureByName(textureName) ;
	if(tex != NULL){
		int index = (tex - textures)/sizeof(Texture);
		textureRefs[index]++;
		return tex;
	}

	for(int i = 0; i < texAlloc; i++){
		if(textureRefs[i] == 0){
			textures[i] = Texture(GL_TEXTURE_2D, textureName);
			textures[i].Load();
			textureRefs[i] = 1;
			return &(textures[i]);
		}
	}

	return NULL;
}
Model* ResourceManager::LoadMesh(string modelName){
	Model* mesh = GetMeshByName(modelName) ;
	if(mesh != NULL){
		int index = (mesh - meshes)/sizeof(Model);
		meshRefs[index]++;
		return mesh;
	}

	for(int i = 0; i < meshAlloc; i++){
		if(meshRefs[i] == 0){
			meshes[i] = Model(modelName);
			meshRefs[i] = 1;
			return &(meshes[i]);
		}
	}

	return NULL;
}

void ResourceManager::Release(Material* mat){
	int index = (mat - materials)/sizeof(Material);
	materialRefs[index]--;

	if(materialRefs[index] == 0){
		mat->Release(this);
	}
}
void ResourceManager::Release(Texture* tex){
	int index = (tex - textures)/sizeof(Texture);
	textureRefs[index]--;

	if(textureRefs[index] == 0){
		tex->Release(this);
	}
}
void ResourceManager::Release(Model* mesh){
	int index = (mesh - meshes)/sizeof(Model);
	meshRefs[index]--;

	meshes[index].name = "";
}

ResourceManager::~ResourceManager(){
	if(materials != nullptr){
		delete[] materials;
	}
	if(meshes != nullptr){
		delete[] meshes;
	}
	if(textures != nullptr){
		delete[] textures;
	}

	if(materialRefs != NULL){ delete[] materialRefs; }
	if(textureRefs != NULL) { delete[] textureRefs;  }
	if(meshRefs != NULL)    { delete[] meshRefs;     }
}