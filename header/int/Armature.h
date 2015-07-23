#ifndef ARMATURE_H
#define ARMATURE_H

#include "BoneTransform.h"
#include "../header/int/Mat4.h"

#include <vector>
#include <string>

using std::vector; using std::string;

struct Model;

#define MAX_BONE_COUNT 32

struct Armature{
	BoneTransform bones[MAX_BONE_COUNT];
	Mat4x4 bindPoses[MAX_BONE_COUNT];
	Model* model;
	int boneCount;

	Armature()
		: boneCount(0){
	}

	BoneTransform* AddBone(BoneTransform* parent);
	void GetBoneMatrices(vector<Mat4x4>& outMatrices);
	void DebugRender();

	BoneTransform* GetBoneByName(const string& name);
};

#endif