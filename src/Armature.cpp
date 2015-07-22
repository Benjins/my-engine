#include "../header/int/Armature.h"
#include "../header/int/Mat4.h"
#include "../header/int/GameObject.h"
#include "../header/int/Model.h"

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

	//bones[0].SetParent(&trans);

	outMatrices.resize(boneCount);
	for(int i = 0; i < boneCount; i++){
		outMatrices[i] = bones[i].LocalToGlobalMatrix();
	}

	//bones[0].SetParent(nullptr);
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

	for(int i = 0; i < boneCount; i++){
		Vector3 pos = bones[i].GlobalPosition();
		Vector3 forward = pos + bones[i].Forward() * 0.5f;
		Vector3 up = pos + bones[i].Up() * 0.5f;
		Vector3 right = pos + bones[i].Right() * 0.5f;

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