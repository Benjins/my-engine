#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include "../ext/Vector2.h"
#include "../ext/Vector3.h"

using std::vector; using std::string;

struct Armature;

#define MAX_BONES_PER_VERT 4

struct Face{
	int v0;
	int v1;
	int v2;
	
	Vector2 uv0;
	Vector2 uv1;
	Vector2 uv2;
	
	Face();
	Face(int _v0, int _v1, int _v2);
};

struct Vertex{
	Vector3 position;
	Vector3 color;
	Vector3 normal;
	Vector3 tangent;
	//Vector3 uv;

	float boneWeights[MAX_BONES_PER_VERT];
	int boneIndices[MAX_BONES_PER_VERT];
	short numBones;

	void AddBone(int index, float weight){
		if(numBones < MAX_BONES_PER_VERT){
			boneWeights[numBones] = weight;
			boneIndices[numBones] = index;
			numBones++;
		}
		else{
			cout << "\n\nError: Too many bones added to vertex, max is " << MAX_BONES_PER_VERT << ". The rest will be discarded.\n";
		}
	}
	
	Vertex();
	Vertex(Vector3 _position);
};

struct Model{
	vector<Vertex> vertices;
	vector<Face> faces;

	string name;
	string fileName;

	Armature* armature;
	
	Model(void);
	Model(const Model& model);
	Model(string fileName);

	~Model();
	
	int GLVertexCount() const;
	
	void ImportFromOBJ(const string& fileName);
	void ImportFromCollada(const string& fileName);

	void CalculateNormals();
	void CalculateTangents();
};

Vertex ParseVertexLine(string line);

Vector2 ParseUVLine(string line);

Face ParseFaceLine(string line, const vector<Vector2>& uvs);

vector<string> SplitStringByDelimiter(string searchIn, string delimiter);

#endif