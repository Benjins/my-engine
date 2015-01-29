#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <string>

using std::string;

struct Texture; struct Material; struct Model; struct GameObject;


struct MaterialManager{
	Material* materials;


	unsigned int matAlloc;

public:
	MaterialManager(int matCount = 25);

	Material* GetMaterialByName(string name);
	int GetMaterialIdByName(string name);
	Material* GetMaterialById(int id);

	Material* LoadMaterial(string matName, string shaderName, string textureName="");

	void Clear();

	~MaterialManager();
};



#endif