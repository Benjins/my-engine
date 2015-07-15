#ifndef ARMATURE_H
#define ARMATURE_H

#include "BoneTransform.h"

#include <vector>
#include <string>

using std::vector; using std::string;

struct Mat4x4;

#define MAX_BONE_COUNT 32

struct Armature{
	BoneTransform bones[MAX_BONE_COUNT];
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