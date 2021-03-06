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

void Armature::Update(float deltaTime){
	time += deltaTime;
	currentBlendTime += deltaTime;

	if(currentBlendTime >= blendTime && currentAnimIndex != targetAnimIndex){
		//printf("Stope blending at CBT='%0.3f'\n", currentBlendTime);
		currentAnimIndex = targetAnimIndex;
		blendTime = 0;
		time = currentBlendTime;
	}

	bool isBlending = currentAnimIndex != targetAnimIndex;

	for(int i = 0; i < boneCount; i++){
		Vector3 currentPos    = anim[currentAnimIndex].boneAnims[i].positionAnim.Evaluate(time);
		Quaternion currentRot = anim[currentAnimIndex].boneAnims[i].rotationAnim.Evaluate(time);

		if(!isBlending){
			//printf("Non-blending eval at %.3f out of %.3f\n", time, anim[currentAnimIndex].boneAnims[i].positionAnim.Length());
			bones[i].position = currentPos;
			bones[i].rotation = currentRot;
		}
		else{
			Vector3 targetPos    = anim[targetAnimIndex].boneAnims[i].positionAnim.Evaluate(currentBlendTime);
			Quaternion targetRot = anim[targetAnimIndex].boneAnims[i].rotationAnim.Evaluate(currentBlendTime);

			if(i == 2){
				//printf("Blending CBT:%0.2f, time:%0.2f, blendTime:%0.2f\n", currentBlendTime, time, blendTime);
			}
			float blendWeight = currentBlendTime / blendTime;
			Vector3 blendedPos    = currentPos * (1 - blendWeight) + targetPos * blendWeight;
			Quaternion blendedRot = currentRot * (1 - blendWeight) + targetRot * blendWeight;

			bones[i].position = blendedPos;
			bones[i].rotation = blendedRot;
		}
	}

	IKPass();
}

void Armature::IKPass(){
	for(const IKConstraint& ik : ikConstraints){
		vector<pair<Quaternion, int>> jointAngles;
		jointAngles.reserve(ik.constraintLength);

		Vector3 targetPos = model->gameObject->transform.GlobalToLocalMatrix() * ik.position;

		BoneTransform* ikBone = &bones[ik.boneIndex];
		BoneTransform* currBone = ikBone->GetBoneParent();
		for(int i = 0; currBone != nullptr && i < ik.constraintLength; i++){
			jointAngles.emplace_back(currBone->rotation, currBone - bones);
			currBone = currBone->GetBoneParent();
		}

		float gradMult = 0.005f;

		for(int iteration = 0; iteration < 10; iteration++){
			Vector3 currPos = ikBone->GlobalPosition();
			Vector3 deltaPos = targetPos - currPos;

			vector<float> jacobian;
			jacobian.reserve(jointAngles.size() * 4);
			
			for(int i = 0; i < jointAngles.size() * 4; i++){
				Quaternion currAngle = jointAngles[i / 4].first;
				float* angleFltCast = reinterpret_cast<float*>(&currAngle);

				const float h = 0.001f;
				float mag = 1 + angleFltCast[i % 4] * h * 2 + h*h;
				angleFltCast[i % 4] += h;
				currAngle = currAngle / mag;

				Quaternion formerAngle = jointAngles[i / 4].first;
				bones[jointAngles[i / 4].second].rotation = currAngle;

				Vector3 newPos = ikBone->GlobalPosition();
				Vector3 newDeltaPos = targetPos - newPos;
				float benefit = newDeltaPos.Magnitude() - deltaPos.Magnitude();
				float partialDeriv = benefit/h;
				jacobian.push_back(partialDeriv);

				bones[jointAngles[i / 4].second].rotation = formerAngle;	
			}

			vector<pair<Quaternion,int>> jointAnglesCopy = jointAngles;

			while(gradMult > 0.00000001f){
				for(int i = 0; i < jointAngles.size();  i++){
					Quaternion quat = jointAngles[i].first;
					Quaternion toAdd = Quaternion(jacobian[4*i], jacobian[4*i+1], jacobian[4*i+2], jacobian[4*i+3]); 
				
					Quaternion newRot = (quat - toAdd * gradMult).Normalized();

					Quaternion diff = newRot - quat;

					jointAngles[i].first = newRot;
					bones[jointAngles[i].second].rotation = newRot;
				}

				Vector3 newPos = bones[ik.boneIndex].GlobalPosition();
				Vector3 newDelta = targetPos - newPos;
				float prevDeltaMag = deltaPos.MagnitudeSquared();
				float newDeltaMag = newDelta.MagnitudeSquared();

				if(newDeltaMag < prevDeltaMag){
					//printf("Iteration[%d]: Found better set of rotations.\n", iteration);
					break;
				}
				else{
					for(int i = 0; i < jointAnglesCopy.size();  i++){
						bones[jointAnglesCopy[i].second].rotation = jointAnglesCopy[i].first;
					}

					gradMult *= 0.5f;
				}
			}
		}
	}
}


BoneTransform* Armature::GetBoneByName(const string& name){
	for(int i = 0; i < boneCount; i++){
		if(bones[i].name == name){
			return &bones[i];
		}
	}

	return nullptr;
}

int Armature::GetAnimationIndexByName(const string& name){
	for(int i = 0; i < animCount; i++){
		if(anim[i].stateName == name){
			return i;
		}
	}

	return -1;
}

void Armature::BlendTo(const string& animName, float time){
	int index = GetAnimationIndexByName(animName);
	if(index != -1){
		BlendToIndex(index, time);
	}
	else{
		cout << "\n\nError: trying to blend to invalid state: '" << animName << "'" << endl;
	}
}

void Armature::BlendToIndex(int animIndex, float time){
	if(targetAnimIndex != animIndex){
		targetAnimIndex = animIndex;
		blendTime = time;
		currentBlendTime = 0;
	}
}

void Armature::ParseStateString(const string& states){
	vector<string> stateList = SplitStringByDelimiter(states, ",");
	int stateCount = 1;
	for(const string& state : stateList){
		vector<string> parsedState = SplitStringByDelimiter(state, ":");
		if(parsedState.size() != 2){
			cout << "Warning: Invalid state description: '" << state << "'" << endl;
		}
		else{
			const string& stateName = parsedState[0];
			const string& stateTimes = parsedState[1];
			vector<string> stateTimesSplit = SplitStringByDelimiter(stateTimes, "-");
			if(stateTimesSplit.size() != 2){
				cout << "Warning: Invalid state times: '" << stateTimes << "'" << endl;
				break;
			}

			float timeMin = atof(stateTimesSplit[0].c_str());
			float timeMax = atof(stateTimesSplit[1].c_str());

			anim[stateCount].stateName = stateName;
			for(int i = 0; i < boneCount; i++){
				anim[stateCount].boneAnims[i].positionAnim.AddKeyFrame(anim[0].boneAnims[i].positionAnim.Evaluate(timeMin), 0);
				anim[stateCount].boneAnims[i].positionAnim.AddKeyFrame(anim[0].boneAnims[i].positionAnim.Evaluate(timeMax), timeMax - timeMin);
				for(KeyFrame<Vector3> frame : anim[0].boneAnims[i].positionAnim.keyFrames){
					if(frame.time > timeMin && frame.time < timeMax){
						anim[stateCount].boneAnims[i].positionAnim.AddKeyFrame(frame.value, frame.time - timeMin);
					}
				}

				anim[stateCount].boneAnims[i].rotationAnim.AddKeyFrame(anim[0].boneAnims[i].rotationAnim.Evaluate(timeMin), 0);
				anim[stateCount].boneAnims[i].rotationAnim.AddKeyFrame(anim[0].boneAnims[i].rotationAnim.Evaluate(timeMax), timeMax - timeMin);
				for(KeyFrame<Quaternion> frame : anim[0].boneAnims[i].rotationAnim.keyFrames){
					if(frame.time > timeMin && frame.time < timeMax){
						anim[stateCount].boneAnims[i].rotationAnim.AddKeyFrame(frame.value, frame.time - timeMin);
					}
				}
			}

			stateCount++;
		}
	}

	animCount = stateCount;
}

string Armature::EncodeStateString(){
	string encoded="";
	bool first = true;
	for(int i = 1; i < animCount; i++){
		if(!first){
			encoded += ",";
		}

		encoded += (anim[i].stateName + ":");

		float start = anim[i].boneAnims[0].positionAnim.keyFrames[0].time;
		float end = start + anim[i].boneAnims[0].positionAnim.Length();

		encoded += (to_string(start) + "-" + to_string(end));
		first = false;
	}

	return encoded;
}

