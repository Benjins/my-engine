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
	SC_Transform trans;
	trans.rotation = Quaternion(X_AXIS, -3.14159265f/2);
	trans.position = Vector3(0,0,-0.3091092f);

	outMatrices.resize(boneCount);
	for(int i = 0; i < boneCount; i++){
		Mat4x4 localToGlobal = bones[i].LocalToGlobalMatrix();
		//localToGlobal.SetColumn(3, Vector4(bones[i].position, 1));
		outMatrices[i] =  localToGlobal * bindPoses[i];
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

	//trans.SetParent(&model->gameObject->transform);
	bones[0].SetParent(&model->gameObject->transform);

	Vector3 objPos = model->gameObject->transform.GlobalPosition();
	Vector3 objForward = objPos + model->gameObject->transform.Forward();
	Vector3 objUp = objPos + model->gameObject->transform.Up();
	Vector3 objRight = objPos + model->gameObject->transform.Right();

	glUniform4f(colorLoc, 0, 0, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(objPos.x, objPos.y, objPos.z);
	glVertex3f(objForward.x, objForward.y, objForward.z);
	glEnd();

	glUniform4f(colorLoc, 0, 1, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(objPos.x, objPos.y, objPos.z);
	glVertex3f(objUp.x, objUp.y, objUp.z);
	glEnd();

	glUniform4f(colorLoc, 1, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(objPos.x, objPos.y, objPos.z);
	glVertex3f(objRight.x, objRight.y, objRight.z);
	glEnd();

	vector<Mat4x4> boneMats;
	GetBoneMatrices(boneMats);

	for(int i = 0; i < boneCount; i++){
		Vector4 pos4 = boneMats[i] * Vector4(0,0,0,1);
		Vector3 pos = Vector3(pos4.w, pos4.x, pos4.y);

		Vector4 for4 = boneMats[i] * Vector4(0,0,1,0);
		Vector3 forward = pos + Vector3(for4.w, for4.x, for4.y);

		Vector4 up4 = boneMats[i] * Vector4(0,1,0,0);
		Vector3 up = pos + Vector3(up4.w, up4.x, up4.y);

		Vector4 right4 = boneMats[i] * Vector4(1,0,0,0);
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