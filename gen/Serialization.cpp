#include "../header/int/UserComps.h"

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
XMLElement BoxCollider::Serialize(){
XMLElement elem;
elem.name = "BoxCollider";
elem.AddAttribute("position", EncodeVector3(position));
elem.AddAttribute("size", EncodeVector3(size));
return elem;
}
void BoxCollider::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("position");
if(iter != elem.attributeMap.end()){position = ParseVector3(iter->second);}
iter = elem.attributeMap.find("size");
if(iter != elem.attributeMap.end()){size = ParseVector3(iter->second);}

}
Component* BoxCollider::Clone(){
BoxCollider* newComp = new BoxCollider();
newComp->position = position;
newComp->size = size;
return newComp;
}
XMLElement SphereCollider::Serialize(){
XMLElement elem;
elem.name = "SphereCollider";
elem.AddAttribute("position", EncodeVector3(position));
elem.AddAttribute("radius", to_string(radius));
return elem;
}
void SphereCollider::Deserialize(const XMLElement& elem){
auto iter = elem.attributeMap.begin();iter = elem.attributeMap.find("position");
if(iter != elem.attributeMap.end()){position = ParseVector3(iter->second);}
iter = elem.attributeMap.find("radius");
if(iter != elem.attributeMap.end()){radius = atof(iter->second.c_str());}

}
Component* SphereCollider::Clone(){
SphereCollider* newComp = new SphereCollider();
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
