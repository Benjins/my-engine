#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <string>

using std::string;

struct Texture; struct Material; struct Model; struct GameObject;


struct MaterialManager{
	Material* materials;
	Texture* textures;

	unsigned int matAlloc;
	unsigned int texAlloc;

public:
	MaterialManager(int matCount = 40);

	Material* GetMaterialByName(string name);
	int GetMaterialIdByName(string name);
	Material* GetMaterialById(int id);

	Material* LoadMaterial(string matName, string shaderName, string textureName="", string bumpMapName = "");

	Texture* LoadTexture(const string& fileName, bool forceInstance = false);
	Texture* GetTextureByFileName(const string& fileName);

	void Clear();

	~MaterialManager();
};


#endif