#include "../header/int/UserComps.h"

XMLElement LadderComponent::Serialize(){
XMLElement elem;
elem.name = "LadderComponent";
return elem;
}
void LadderComponent::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();
}
Component* LadderComponent::Clone(){
LadderComponent* newComp = new LadderComponent();
return newComp;
}
XMLElement DoorComponent::Serialize(){
XMLElement elem;
elem.name = "DoorComponent";
elem.AddAttribute("isLocked", (isLocked ? "T" : "F"));
elem.AddAttribute("isOpen", (isOpen ? "T" : "F"));
return elem;
}
void DoorComponent::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("isLocked");
if(iter != elem.attributeMap.end()){isLocked = iter->second == "T";}
iter = elem.attributeMap.find("isOpen");
if(iter != elem.attributeMap.end()){isOpen = iter->second == "T";}

}
Component* DoorComponent::Clone(){
DoorComponent* newComp = new DoorComponent();
newComp->isLocked = isLocked;
newComp->isOpen = isOpen;
return newComp;
}
XMLElement IKAnimTestComp::Serialize(){
XMLElement elem;
elem.name = "IKAnimTestComp";
elem.AddAttribute("doIk", (doIk ? "T" : "F"));
return elem;
}
Component* IKAnimTestComp::Clone(){
IKAnimTestComp* newComp = new IKAnimTestComp();
newComp->doIk = doIk;
return newComp;
}
XMLElement AnimationControlTest::Serialize(){
XMLElement elem;
elem.name = "AnimationControlTest";
elem.AddAttribute("timeOut", to_string(timeOut));
elem.AddAttribute("isIdle", (isIdle ? "T" : "F"));
return elem;
}
Component* AnimationControlTest::Clone(){
AnimationControlTest* newComp = new AnimationControlTest();
newComp->timeOut = timeOut;
newComp->isIdle = isIdle;
return newComp;
}
XMLElement AudioComponent::Serialize(){
XMLElement elem;
elem.name = "AudioComponent";
elem.AddAttribute("clipName", clipName);
elem.AddAttribute("volume", to_string(volume));
elem.AddAttribute("loop", (loop ? "T" : "F"));
elem.AddAttribute("autoPlay", (autoPlay ? "T" : "F"));
return elem;
}
void AudioComponent::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("clipName");
if(iter != elem.attributeMap.end()){clipName = iter->second;}
iter = elem.attributeMap.find("volume");
if(iter != elem.attributeMap.end()){volume = atof(iter->second.c_str());}
iter = elem.attributeMap.find("loop");
if(iter != elem.attributeMap.end()){loop = iter->second == "T";}
iter = elem.attributeMap.find("autoPlay");
if(iter != elem.attributeMap.end()){autoPlay = iter->second == "T";}

}
Component* AudioComponent::Clone(){
AudioComponent* newComp = new AudioComponent();
newComp->clipName = clipName;
newComp->volume = volume;
newComp->loop = loop;
newComp->autoPlay = autoPlay;
return newComp;
}
XMLElement CameraControl::Serialize(){
XMLElement elem;
elem.name = "CameraControl";
elem.AddAttribute("speed", to_string(speed));
elem.AddAttribute("velocity", to_string(velocity));
elem.AddAttribute("isGrounded", (isGrounded ? "T" : "F"));
elem.AddAttribute("health", to_string(health));
elem.AddAttribute("stepDelay", to_string(stepDelay));
elem.AddAttribute("timeMoving", to_string(timeMoving));
elem.AddAttribute("characterHeight", to_string(characterHeight));
elem.AddAttribute("groundedAdjustment", to_string(groundedAdjustment));
elem.AddAttribute("prevX", to_string(prevX));
elem.AddAttribute("prevY", to_string(prevY));
elem.AddAttribute("xRot", to_string(xRot));
elem.AddAttribute("yRot", to_string(yRot));
elem.AddAttribute("ladderSpeed", to_string(ladderSpeed));
return elem;
}
void CameraControl::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("speed");
if(iter != elem.attributeMap.end()){speed = atof(iter->second.c_str());}
iter = elem.attributeMap.find("velocity");
if(iter != elem.attributeMap.end()){velocity = atof(iter->second.c_str());}
iter = elem.attributeMap.find("isGrounded");
if(iter != elem.attributeMap.end()){isGrounded = iter->second == "T";}
iter = elem.attributeMap.find("health");
if(iter != elem.attributeMap.end()){health = atof(iter->second.c_str());}
iter = elem.attributeMap.find("stepDelay");
if(iter != elem.attributeMap.end()){stepDelay = atof(iter->second.c_str());}
iter = elem.attributeMap.find("timeMoving");
if(iter != elem.attributeMap.end()){timeMoving = atof(iter->second.c_str());}
iter = elem.attributeMap.find("characterHeight");
if(iter != elem.attributeMap.end()){characterHeight = atof(iter->second.c_str());}
iter = elem.attributeMap.find("groundedAdjustment");
if(iter != elem.attributeMap.end()){groundedAdjustment = atof(iter->second.c_str());}
iter = elem.attributeMap.find("prevX");
if(iter != elem.attributeMap.end()){prevX = atoi(iter->second.c_str());}
iter = elem.attributeMap.find("prevY");
if(iter != elem.attributeMap.end()){prevY = atoi(iter->second.c_str());}
iter = elem.attributeMap.find("xRot");
if(iter != elem.attributeMap.end()){xRot = atof(iter->second.c_str());}
iter = elem.attributeMap.find("yRot");
if(iter != elem.attributeMap.end()){yRot = atof(iter->second.c_str());}
iter = elem.attributeMap.find("ladderSpeed");
if(iter != elem.attributeMap.end()){ladderSpeed = atof(iter->second.c_str());}

}
Component* CameraControl::Clone(){
CameraControl* newComp = new CameraControl();
newComp->speed = speed;
newComp->velocity = velocity;
newComp->isGrounded = isGrounded;
newComp->health = health;
newComp->stepDelay = stepDelay;
newComp->timeMoving = timeMoving;
newComp->characterHeight = characterHeight;
newComp->groundedAdjustment = groundedAdjustment;
newComp->prevX = prevX;
newComp->prevY = prevY;
newComp->xRot = xRot;
newComp->yRot = yRot;
newComp->ladderSpeed = ladderSpeed;
return newComp;
}
XMLElement BulletComponent::Serialize(){
XMLElement elem;
elem.name = "BulletComponent";
elem.AddAttribute("speed", to_string(speed));
elem.AddAttribute("time", to_string(time));
elem.AddAttribute("timeLimit", to_string(timeLimit));
return elem;
}
void BulletComponent::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("speed");
if(iter != elem.attributeMap.end()){speed = atof(iter->second.c_str());}
iter = elem.attributeMap.find("time");
if(iter != elem.attributeMap.end()){time = atof(iter->second.c_str());}
iter = elem.attributeMap.find("timeLimit");
if(iter != elem.attributeMap.end()){timeLimit = atof(iter->second.c_str());}

}
Component* BulletComponent::Clone(){
BulletComponent* newComp = new BulletComponent();
newComp->speed = speed;
newComp->time = time;
newComp->timeLimit = timeLimit;
return newComp;
}
XMLElement PathNodeComponent::Serialize(){
XMLElement elem;
elem.name = "PathNodeComponent";
return elem;
}
Component* PathNodeComponent::Clone(){
PathNodeComponent* newComp = new PathNodeComponent();
return newComp;
}
XMLElement LightComponent::Serialize(){
XMLElement elem;
elem.name = "LightComponent";
elem.AddAttribute("intensity", to_string(intensity));
elem.AddAttribute("isDirectional", (isDirectional ? "T" : "F"));
return elem;
}
void LightComponent::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("intensity");
if(iter != elem.attributeMap.end()){intensity = atof(iter->second.c_str());}
iter = elem.attributeMap.find("isDirectional");
if(iter != elem.attributeMap.end()){isDirectional = iter->second == "T";}

}
Component* LightComponent::Clone(){
LightComponent* newComp = new LightComponent();
newComp->intensity = intensity;
newComp->isDirectional = isDirectional;
return newComp;
}
XMLElement FireGun::Serialize(){
XMLElement elem;
elem.name = "FireGun";
elem.AddAttribute("bulletForce", to_string(bulletForce));
return elem;
}
void FireGun::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("bulletForce");
if(iter != elem.attributeMap.end()){bulletForce = atof(iter->second.c_str());}

}
Component* FireGun::Clone(){
FireGun* newComp = new FireGun();
newComp->bulletForce = bulletForce;
return newComp;
}
XMLElement MatChangeOnHit::Serialize(){
XMLElement elem;
elem.name = "MatChangeOnHit";
return elem;
}
Component* MatChangeOnHit::Clone(){
MatChangeOnHit* newComp = new MatChangeOnHit();
return newComp;
}
XMLElement EnemyComp::Serialize(){
XMLElement elem;
elem.name = "EnemyComp";
elem.AddAttribute("speed", to_string(speed));
elem.AddAttribute("health", to_string(health));
elem.AddAttribute("maxHealth", to_string(maxHealth));
elem.AddAttribute("reloadTime", to_string(reloadTime));
return elem;
}
void EnemyComp::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("speed");
if(iter != elem.attributeMap.end()){speed = atof(iter->second.c_str());}
iter = elem.attributeMap.find("health");
if(iter != elem.attributeMap.end()){health = atoi(iter->second.c_str());}
iter = elem.attributeMap.find("maxHealth");
if(iter != elem.attributeMap.end()){maxHealth = atoi(iter->second.c_str());}
iter = elem.attributeMap.find("reloadTime");
if(iter != elem.attributeMap.end()){reloadTime = atof(iter->second.c_str());}

}
Component* EnemyComp::Clone(){
EnemyComp* newComp = new EnemyComp();
newComp->speed = speed;
newComp->health = health;
newComp->maxHealth = maxHealth;
newComp->reloadTime = reloadTime;
return newComp;
}
XMLElement BoxCollider::Serialize(){
XMLElement elem;
elem.name = "BoxCollider";
elem.AddAttribute("isTrigger", (isTrigger ? "T" : "F"));
elem.AddAttribute("position", EncodeVector3(position));
elem.AddAttribute("size", EncodeVector3(size));
return elem;
}
void BoxCollider::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("isTrigger");
if(iter != elem.attributeMap.end()){isTrigger = iter->second == "T";}
iter = elem.attributeMap.find("position");
if(iter != elem.attributeMap.end()){position = ParseVector3(iter->second);}
iter = elem.attributeMap.find("size");
if(iter != elem.attributeMap.end()){size = ParseVector3(iter->second);}

}
Component* BoxCollider::Clone(){
BoxCollider* newComp = new BoxCollider();
newComp->isTrigger = isTrigger;
newComp->position = position;
newComp->size = size;
return newComp;
}
XMLElement SphereCollider::Serialize(){
XMLElement elem;
elem.name = "SphereCollider";
elem.AddAttribute("isTrigger", (isTrigger ? "T" : "F"));
elem.AddAttribute("position", EncodeVector3(position));
elem.AddAttribute("radius", to_string(radius));
return elem;
}
void SphereCollider::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("isTrigger");
if(iter != elem.attributeMap.end()){isTrigger = iter->second == "T";}
iter = elem.attributeMap.find("position");
if(iter != elem.attributeMap.end()){position = ParseVector3(iter->second);}
iter = elem.attributeMap.find("radius");
if(iter != elem.attributeMap.end()){radius = atof(iter->second.c_str());}

}
Component* SphereCollider::Clone(){
SphereCollider* newComp = new SphereCollider();
newComp->isTrigger = isTrigger;
newComp->position = position;
newComp->radius = radius;
return newComp;
}
XMLElement RigidBody::Serialize(){
XMLElement elem;
elem.name = "RigidBody";
elem.AddAttribute("mass", to_string(mass));
elem.AddAttribute("useGravity", (useGravity ? "T" : "F"));
elem.AddAttribute("isKinematic", (isKinematic ? "T" : "F"));
elem.AddAttribute("time", to_string(time));
return elem;
}
void RigidBody::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("mass");
if(iter != elem.attributeMap.end()){mass = atof(iter->second.c_str());}
iter = elem.attributeMap.find("useGravity");
if(iter != elem.attributeMap.end()){useGravity = iter->second == "T";}
iter = elem.attributeMap.find("isKinematic");
if(iter != elem.attributeMap.end()){isKinematic = iter->second == "T";}
iter = elem.attributeMap.find("time");
if(iter != elem.attributeMap.end()){time = atof(iter->second.c_str());}

}
Component* RigidBody::Clone(){
RigidBody* newComp = new RigidBody();
newComp->mass = mass;
newComp->useGravity = useGravity;
newComp->isKinematic = isKinematic;
newComp->time = time;
return newComp;
}
