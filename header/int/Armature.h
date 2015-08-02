#ifndef ARMATURE_H
#define ARMATURE_H

#include "BoneTransform.h"
#include "Mat4.h"
#include "ArmatureAnimation.h"

#include <vector>
#include <string>

using std::vector; using std::string;

struct Model;

struct Armature{
	BoneTransform bones[MAX_BONE_COUNT];
	Mat4x4 bindPoses[MAX_BONE_COUNT];
	ArmatureAnimation anim;

	Model* model;
	int boneCount;

	float time;

	Armature()
	: boneCount(0)
	, time(0.0f)
	, model(nullptr){
	}

	BoneTransform* AddBone(BoneTransform* parent);
	void GetBoneMatrices(vector<Mat4x4>& outMatrices);
	void DebugRender();

	void Update(float deltaTime);

	BoneTransform* GetBoneByName(const string& name);
};

#endif