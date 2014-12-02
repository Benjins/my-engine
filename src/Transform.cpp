#include "../header/int/Transform.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"

SC_Transform::SC_Transform(){
	position = Vector3(0,0,0);
	rotation = QUAT_IDENTITY;
	scale = Vector3(1,1,1);
	
	parent = NULL;
}

SC_Transform::SC_Transform(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, SC_Transform* _parent){
	position  = _position;
	rotation  = _rotation;
	scale     = _scale;
	parent    = _parent;
}

Vector3 SC_Transform::Forward() const{
	if(parent != NULL){
		return Rotate(parent->Forward(), rotation);
	}
	return Rotate(Z_AXIS, rotation);
}

Vector3 SC_Transform::Up() const{
	if(parent != NULL){
		return Rotate(parent->Up(), rotation);
	}
	return Rotate(Y_AXIS, rotation);
}

Vector3 SC_Transform::Right() const{
	if(parent != NULL){
		return Rotate(parent->Right(), rotation);
	}
	return Rotate(X_AXIS, rotation);
}

Mat4x4 SC_Transform::LocalToGlobalMatrix() const{
	Mat4x4 transMat;

	Mat4x4 linMat;
	Mat4x4 affMat;

	linMat.SetColumn(0, Vector4(Rotate(X_AXIS * scale.x, rotation), 0));
	linMat.SetColumn(1, Vector4(Rotate(Y_AXIS * scale.y, rotation), 0));
	linMat.SetColumn(2, Vector4(Rotate(Z_AXIS * scale.z, rotation), 0));
	linMat.SetColumn(3, Vector4(0,0,0,1));

	affMat.SetRow(0, Vector4(X_AXIS, position.x));
	affMat.SetRow(1, Vector4(Y_AXIS, position.y));
	affMat.SetRow(2, Vector4(Z_AXIS, position.z));
	affMat.SetRow(3, Vector4(0,0,0,1));

	transMat = affMat * linMat;

	if(parent != NULL){
		return parent->LocalToGlobalMatrix() * transMat;
	}

	return transMat;
}

Mat4x4 SC_Transform::GlobalToLocalMatrix() const{
	Mat4x4 transMat;

	Mat4x4 linMat;
	Mat4x4 affMat;

	linMat.SetColumn(0, Vector4(Rotate(X_AXIS, rotation.Conjugate()) / scale.x, 0));
	linMat.SetColumn(1, Vector4(Rotate(Y_AXIS, rotation.Conjugate()) / scale.y, 0));
	linMat.SetColumn(2, Vector4(Rotate(Z_AXIS, rotation.Conjugate()) / scale.z, 0));
	linMat.SetColumn(3, Vector4(0,0,0,1));

	affMat.SetRow(0, Vector4(X_AXIS, -position.x));
	affMat.SetRow(1, Vector4(Y_AXIS, -position.y));
	affMat.SetRow(2, Vector4(Z_AXIS, -position.z));
	affMat.SetRow(3, Vector4(0,0,0,1));

	transMat = linMat * affMat;

	if(parent != NULL){
		return parent->GlobalToLocalMatrix() * transMat;
	}

	return transMat;
}

Mat4x4 SC_Transform::GetCameraMatrix() const{
	Mat4x4 transMat;

	Mat4x4 linMat;
	Mat4x4 affMat;

	linMat.SetColumn(0, Vector4(Rotate(X_AXIS / scale.x, rotation.Conjugate()), 0));
	linMat.SetColumn(1, Vector4(Rotate(Y_AXIS / scale.y, rotation.Conjugate()), 0));
	linMat.SetColumn(2, Vector4(Rotate(Z_AXIS / scale.z, rotation.Conjugate()), 0));
	linMat.SetColumn(3, Vector4(0,0,0,1));

	affMat.SetRow(0, Vector4(X_AXIS, -position.x));
	affMat.SetRow(1, Vector4(Y_AXIS, -position.y));
	affMat.SetRow(2, Vector4(Z_AXIS, -position.z));
	affMat.SetRow(3, Vector4(0,0,0,1));

	transMat = linMat * affMat;

	return transMat;
}

SC_Transform SC_Transform::GetInverse() const{
	return SC_Transform(position*-1, 
						rotation.Conjugate(), 
						Vector3(1/scale.x, 1/scale.y, 1/scale.z));
} 

Vector3 SC_Transform::GlobalPosition() const{
	if(parent == NULL){
		return position;
	}
	else{
		return parent->LocalToGlobal(position);
	}
}

Vector3 SC_Transform::GlobalToLocal(const Vector3& global) const{
	Vector3 localVec = global;
	if(parent != NULL){
		localVec = parent->GlobalToLocal(global);
	}
    localVec = localVec - position;
    localVec = Rotate(localVec, rotation.Conjugate());
    localVec = localVec.Scaled(Vector3( 1 / scale.x,
                                        1 / scale.y,
                                        1 / scale.z));

    return localVec;
}

Vector3 SC_Transform::LocalToGlobal(const Vector3& local) const{
    Vector3 globalVec = local;
	if(parent != NULL){
		globalVec = parent->LocalToGlobal(local);
	}
    globalVec = globalVec.Scaled(scale);
    globalVec = Rotate(globalVec, rotation);
    globalVec = globalVec + position;

    return globalVec;
}

