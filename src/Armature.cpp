#include "../header/int/Armature.h"
#include "../header/int/Mat4.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

BoneTransform* Armature::AddBone(BoneTransform* parent){
	if(boneCount >= MAX_BONE_COUNT){
		cout << "\n\nError: At max bone count.\n";
		return nullptr;
	}
	BoneTransform* newBone = &bones[boneCount];
	newBone->SetParent(parent);
	boneCount++;
	return newBone;
}

void Armature::GetBoneMatrices(vector<Mat4x4>& outMatrices){
	outMatrices.resize(boneCount);
	for(int i = 0; i < boneCount; i++){
		outMatrices[i] = bones[i].LocalToGlobalMatrix();
	}
}

void Armature::DebugRender(){
	glBegin(GL_LINES);
	{
		for(int i = 0; i < boneCount; i++){
			if(bones[i].GetBoneParent() != nullptr){
				Vector3 pos = bones[i].GlobalPosition();
				Vector3 forward = pos + bones[i].Forward();
				Vector3 up = pos + bones[i].Up();
				Vector3 right = pos + bones[i].Right();

				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(forward.x, forward.y, forward.z);

				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(up.x, up.y, up.z);

				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(right.x, right.y, right.z);
			}
		}
	}
	glEnd();
}

BoneTransform* Armature::GetBoneByName(const string& name){
	for(int i = 0; i < boneCount; i++){
		if(bones[i].name == name){
			return &bones[i];
		}
	}

	return nullptr;
}