#include "../header/int/Armature.h"
#include "../header/int/GameObject.h"
#include "../header/int/Model.h"
#include "../header/int/Vector4.h"

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
		Mat4x4 localToGlobal = bones[i].LocalToGlobalMatrix();
		outMatrices[i] =  localToGlobal * bindPoses[i];//.GetInverse();
		int xy = 0;
	}
}

void Armature::DebugRender(){
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
	GLint colorLoc = glGetUniformLocation(prog, "_color");

	glLineWidth(4);

	SC_Transform trans;
	trans.rotation = Quaternion(X_AXIS, -3.14159265f/2);
	trans.position = Vector3(0,0,-0.3091092f);

	vector<Mat4x4> boneMats;
	GetBoneMatrices(boneMats);
	boneMats.resize(boneMats.size() + 1);

	Mat4x4 modelMatrix = model->gameObject->transform.LocalToGlobalMatrix();

	for(int i = 0; i < boneCount; i++){
		Mat4x4 matrix = modelMatrix * boneMats[i];

		Vector4 pos4 = matrix * Vector4(0,0,0,1);
		Vector3 pos = Vector3(pos4.w, pos4.x, pos4.y);

		Vector4 for4 = matrix * Vector4(0,0,1,0);
		Vector3 forward = pos + Vector3(for4.w, for4.x, for4.y);

		Vector4 up4 = matrix * Vector4(0,1,0,0);
		Vector3 up = pos + Vector3(up4.w, up4.x, up4.y);

		Vector4 right4 = matrix * Vector4(1,0,0,0);
		Vector3 right = pos + Vector3(right4.w, right4.x, right4.y);

		glUniform4f(colorLoc, 0, 0, 1, 1);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(forward.x, forward.y, forward.z);
		glEnd();

		glUniform4f(colorLoc, 0, 1, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(up.x, up.y, up.z);
		glEnd();

		glUniform4f(colorLoc, 1, 0, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(right.x, right.y, right.z);
		glEnd();
	}
	

	bones[0].SetParent(nullptr);
	trans.SetParent(nullptr);
}

BoneTransform* Armature::GetBoneByName(const string& name){
	for(int i = 0; i < boneCount; i++){
		if(bones[i].name == name){
			return &bones[i];
		}
	}

	return nullptr;
}