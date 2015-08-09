#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <string>

using std::string;

struct Texture; struct Material; struct Model; struct GameObject;


struct MaterialManager{
	Material* materials;
	Texture* textures;
	Model* models;

	unsigned int matAlloc;
	unsigned int texAlloc;
	unsigned int meshAlloc;

public:
	MaterialManager(int matCount = 40);

	Material* GetMaterialByName(string name);
	int GetMaterialIdByName(string name);
	Material* GetMaterialById(int id);

	Material* GetFreeMaterial();
	Material* LoadMaterial(string matName, string shaderName, string textureName="", string bumpMapName = "");

	Texture* LoadTexture(const string& fileName, bool forceInstance = false);
	Texture* GetTextureByFileName(const string& fileName);

	Model* LoadMesh(const string& fileName, bool forceInstance = false);
	Model* GetMeshByName(const string& name);

	void Clear();

	~MaterialManager();
};


#endif