#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <string>

using std::string;

/*
#if !defined(_WIN32) && !defined(_WIN64)
namespace ResourceType{
#endif
	enum ResourceType{
		TextureRes,
		MaterialRes,
		MeshRes,
		GameObjectRes
	};
#if !defined(_WIN32) && !defined(_WIN64)
}
#endif

*/

struct Texture; struct Material; struct Model; struct GameObject;

/*
struct ResourceManager{
	Material* materials;
	Texture* textures;
	Model* meshes;

private:
	unsigned int matAlloc;
	unsigned int texAlloc;
	unsigned int meshAlloc;

	unsigned int* materialRefs;
	unsigned int* textureRefs;
	unsigned int* meshRefs;

	void ReserveMaterials(int newMatCount);
	void ReserveTextures(int newTexCount);
	void ReserveMeshes(int newMeshCount);
public:
	ResourceManager(int matCount = 25, int textureCount = 25, int meshCount = 25);

	Material* GetMaterialByName(string name);
	int GetMaterialIdByName(string name);
	Material* GetMaterialById(int id);

	Texture* GetTextureByName(string name);
	int GetTextureIdByName(string name);
	Texture* GetTextureById(int id);

	Model* GetMeshByName(string name);
	int GetMeshIdByName(string name);
	Model* GetMeshById(int id);

	Material* LoadMaterial(string shaderName, string textureName, bool forceInstance = true, bool forceInstanceTexture = false);
	Texture* LoadTexture(string textureName, bool forceInstance = true);
	Model* LoadMesh(string modelName, bool forceInstance = true);

	void Release(Material* mat);
	void Release(Texture* mat);
	void Release(Model* mat);

	bool IsReleased(Material* mat);
	bool IsReleased(Texture* mat);
	bool IsReleased(Model* mat);

	~ResourceManager();
};
*/

struct MaterialManager{
	Material* materials;
private:
	unsigned int matAlloc;

public:
	MaterialManager(int matCount = 25);

	Material* GetMaterialByName(string name);
	int GetMaterialIdByName(string name);
	Material* GetMaterialById(int id);

	Material* LoadMaterial(string matName, string shaderName, string textureName="");
	~MaterialManager();
};



#endif