#include "../header/int/Transform.h"

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
	return LocalToGlobal(Z_AXIS);
}

Vector3 SC_Transform::Up() const{
	return LocalToGlobal(Y_AXIS);
}

Vector3 SC_Transform::Right() const{
	return LocalToGlobal(X_AXIS);
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
		globalVec = parent->GlobalToLocal(local);
	}
    globalVec = globalVec.Scaled(scale);
    globalVec = Rotate(globalVec, rotation);
    globalVec = globalVec + position;

    return globalVec;
}
