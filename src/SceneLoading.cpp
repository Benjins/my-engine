#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"
#include "../header/int/GuiElement.h"
#include "../header/int/UserComps.h"
#include "../header/ext/simple-xml.h"

struct FireGun;

#define DEFINE_USER_COMPONENT(compName) if(name == #compName){return new compName ();}

Component* GetUserDefinedComp(const string& name){
	if(name == ""){
		return nullptr;
	}
	DEFINE_USER_COMPONENT(MatChangeOnHit);
	DEFINE_USER_COMPONENT(FireGun);
	DEFINE_USER_COMPONENT(EnemyComp);
	DEFINE_USER_COMPONENT(CameraControl);
	DEFINE_USER_COMPONENT(LightComponent);
	DEFINE_USER_COMPONENT(AnimationComponent);
	DEFINE_USER_COMPONENT(PathNodeComponent);
	DEFINE_USER_COMPONENT(AudioComponent);
	return nullptr;
}

string EncodeVector3(Vector3 vec){
	return to_string(vec.x) + "," + to_string(vec.y) + "," + to_string(vec.z);
}

string EncodeVector2(Vector2 vec){
	return to_string(vec.x) + "," + to_string(vec.y);
}

string EncodeQuaternion(Quaternion quat){
	return to_string(quat.w) + "," + to_string(quat.x) + "," + to_string(quat.y)+ "," + to_string(quat.z);
}

string EncodeAnimationType(AnimationType type){
	switch(type){
	case AnimationType::Float:
		return "float";
		break;
	case AnimationType::Vector2:
		return "vector2";
		break;
	case AnimationType::Vector3:
		return "vector3";
		break;
	case AnimationType::Quaternion:
		return "quaternion";
		break;
	default:
		return "";
		break;
	}
}

string EncodeAnimationTarget(AnimationTarget target){
	switch(target){
	case AnimationTarget::Position:
		return "position";
		break;
	case AnimationTarget::Scale:
		return "scale";
		break;
	case AnimationTarget::UVOffset:
		return "uvOffset";
		break;
	case AnimationTarget::UVScale:
		return "uvScale";
		break;
	case AnimationTarget::Rotation:
		return "rotation";
		break;
	default:
		return "";
		break;
	}
}

Vector3 ParseVector3(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");
	float x = atof(parts[0].c_str());
	float y = atof(parts[1].c_str());
	float z = atof(parts[2].c_str());

	return Vector3(x,y,z);
}

Vector2 ParseVector2(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");
	float x = atof(parts[0].c_str());
	float y = atof(parts[1].c_str());

	return Vector2(x,y);
}

Quaternion ParseQuaternion(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");

	float w = atof(parts[0].c_str());
	float x = atof(parts[1].c_str());
	float y = atof(parts[2].c_str());
	float z = atof(parts[3].c_str());

	return Quaternion(w,x,y,z);
}

AnimationType ParseAnimationType(string encoded){
	if(encoded == "float"){
		return AnimationType::Float;
	}
	else if(encoded == "vector2"){
		return AnimationType::Vector2;
	}
	else if(encoded == "vector3"){
		return AnimationType::Vector3;
	}
	else if(encoded == "quaternion"){
		return AnimationType::Quaternion;
	}
	else{
		cout << "\nError: could not understand animation type of: '" << encoded << "'.\n";
		return AnimationType::Float;
	}
}

AnimationTarget ParseAnimationTarget(string encoded){
	if(encoded == "position"){
		return AnimationTarget::Position;
	}
	else if(encoded == "scale"){
		return AnimationTarget::Scale;
	}
	else if(encoded == "uvOffset"){
		return AnimationTarget::UVOffset;
	}
	else if(encoded == "uvScale"){
		return AnimationTarget::UVScale;
	}
	else if(encoded == "rotation"){
		return AnimationTarget::Rotation;
	}
	else{
		cout << "\nError: could not understand animation type of: '" << encoded << "'.\n";
		return AnimationTarget::Position;
	}
	
}

void LoadAudioXML(Scene* scene, XMLElement elem){
	AudioClip* clip = scene->audio.AddClip();
	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "fileName"){
			clip->clipFileName = iter->data;
			clip->LoadFromWavFile(iter->data.c_str());
		}
		else if(iter->name == "name"){
			clip->clipName = iter->data;
		}
	}
}

void LoadMaterialXML(Scene* scene, XMLElement elem){
	string matName="", shaderName="",textureName="",bumpMapName="";
	Vector2 uvOffset = Vector2(0,0), uvScale = Vector2(1,1);

	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "name"){
			matName = iter->data;
		}
		else if(iter->name == "shader"){
			shaderName = iter->data;
		}
		else if(iter->name == "texture"){
			textureName = iter->data;
		}
		else if(iter->name == "bumpMap"){
			bumpMapName = iter->data;
		}
		else if(iter->name == "uvScale"){
			uvScale = ParseVector2(iter->data);
		}
		else if(iter->name == "uvOffset"){
			uvOffset = ParseVector2(iter->data);
		}
	}

	Material* mat = scene->resources.LoadMaterial(matName, shaderName, textureName, bumpMapName);
	mat->uvScale = uvScale;
	mat->uvOffset = uvOffset;
}

void Scene::LoadGameObjectXML(const XMLElement& elem){
	GameObject* go = new GameObject();
	go->scene = this;

	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		XMLAttribute attr = *iter;
		if(attr.name == "name"){
			go->name = attr.data;
		}
	}

	for(auto iter = elem.children.begin(); iter != elem.children.end(); iter++){
		XMLElement child = *iter;
		if(child.name == "Transform"){
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;

				if(attr.name == "position"){
					go->transform.position = ParseVector3(attr.data);
				}
				else if(attr.name == "rotation"){
					go->transform.rotation = ParseQuaternion(attr.data);
				}
				else if(attr.name == "scale"){
					go->transform.scale = ParseVector3(attr.data);
				}
				else if(attr.name == "parent"){
					GameObject* parent = FindGameObject(attr.data);

					if(parent != NULL){
						go->transform.SetParent(&(parent->transform));
					}
				}
			}
		}
		else if(child.name == "Material"){
			string shaderName="", textureName="";
			bool instance=true;
			bool newMaterial=true;
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "name"){
					go->AddNamedMaterial(attr.data);
					newMaterial=false;
				}
				else if(attr.name == "shader"){
					shaderName = attr.data;
				}
				else if(attr.name == "texture"){
					textureName = attr.data;
				}
				else if(attr.name == "instance"){
					if(attr.data == "false"){
						instance=false;
					}
				}
			}

			if(newMaterial){
				go->AddMaterial(shaderName,textureName);
			}
		}
		else if(child.name == "Mesh"){
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "source"){
					go->AddMesh(attr.data);
				}
			}
		}
		else if(child.name == "Camera"){
			camera = &(go->transform);
		}
		else if(child.name == "BoxCollider"){
			BoxCollider* col = go->AddComponent<BoxCollider>();
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "position"){
					col->position = ParseVector3(attr.data);
				}
				else if(attr.name == "size"){
					col->size = ParseVector3(attr.data);
				}
			}
		}
		else if(child.name == "SphereCollider"){
			SphereCollider* col = go->AddComponent<SphereCollider>();
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "position"){
					col->position = ParseVector3(attr.data);
				}
				else if(attr.name == "radius"){
					col->radius = atof(attr.data.c_str());
				}
			}
		}
		else{
			Component* userComp = GetUserDefinedComp(iter->name);
			if(userComp != nullptr){
				userComp->Deserialize(*iter);
				go->AddComponent(userComp);
			}
		}
	}

	AddObject(go);
}

void Scene::LoadScene(string fileName){

	RemoveAllObjects(); 

	XMLDocument doc;
	LoadXMLDoc(doc,fileName);

	for(auto iter = doc.contents.begin(); iter != doc.contents.end(); iter++){
		XMLElement topElem = *iter;

		if(topElem.name == "Scene"){
			for(auto iter2 = topElem.children.begin(); iter2 != topElem.children.end(); iter2++){
				XMLElement res = *iter2;

				if(res.name == "GameObject"){
					LoadGameObjectXML(res);
				}
				else if(res.name == "Material"){
					LoadMaterialXML(this, res);
				}
				else if(res.name == "GuiElement"){
					LoadGuiElement(res);
				}
				else if(res.name == "GuiText"){
					LoadGuiText(res);
				}
				else if(res.name == "AudioClip"){
					LoadAudioXML(this, res);
				}
			}
		}
	}

	OnPostLoad();
}


void Scene::SaveScene(string fileName){
	XMLDocument doc;

	XMLElement scene;
	scene.name = "Scene";
	scene.attributes.push_back(XMLAttribute("name", "my-engine"));

	GameObject* bgn = *objects.begin();

	for(int i = 0; i < resources.matAlloc; i++){
		Material* mat = &(resources.materials[i]);
		if(mat->matName != ""){
			XMLElement matElem;
			matElem.name = "Material";

			XMLAttribute attr;

			attr.name = "name";
			attr.data = mat->matName;
			matElem.attributes.push_back(attr);

			attr.name = "shader";
			attr.data = mat->shaderName;
			matElem.attributes.push_back(attr);

			if(mat->mainTexture != NULL){
				attr.name = "texture";
				attr.data = mat->mainTexture->fileName;
				matElem.attributes.push_back(attr);
			}

			if(mat->bumpMap != NULL){
				attr.name = "bumpMap";
				attr.data = mat->bumpMap->fileName;
				matElem.attributes.push_back(attr);
			}

			matElem.attributes.emplace_back("uvScale", EncodeVector2(mat->uvScale));
			matElem.attributes.emplace_back("uvOffset", EncodeVector2(mat->uvOffset));

			scene.children.push_back(matElem);
		}
	}
	
	for(auto iter = guiElements.begin(); iter != guiElements.end(); iter++){
		scene.children.push_back((*iter)->Serialize());
	}

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		GameObject* obj = *iter;
		XMLElement elem;
		elem.name = "GameObject";
		elem.attributes.push_back(XMLAttribute("name", obj->name));

		XMLElement trans;
		trans.name = "Transform";
		SC_Transform* parent = obj->transform.GetParent();
		if(parent != NULL && parent->gameObject != NULL){
			trans.attributes.push_back(XMLAttribute("parent", parent->gameObject->name));
		}
		trans.attributes.push_back(XMLAttribute("position", EncodeVector3(obj->transform.position)));
		trans.attributes.push_back(XMLAttribute("rotation", EncodeQuaternion(obj->transform.rotation)));
		trans.attributes.push_back(XMLAttribute("scale",    EncodeVector3(obj->transform.scale)));
		elem.children.push_back(trans);

		if(camera == &(obj->transform)){
			XMLElement cam;
			cam.name = "Camera";
			elem.children.push_back(cam);
		}

		Material* mat = obj->material;
		if(mat != NULL){
			XMLElement material;
			material.name = "Material";
			material.attributes.push_back(XMLAttribute("name",mat->matName));
			elem.children.push_back(material);
		}
		Model* model = obj->mesh;
		if(model != NULL){
			XMLElement mesh;
			mesh.name = "Mesh";
			mesh.attributes.push_back(XMLAttribute("source",model->name));
			elem.children.push_back(mesh);
		}
		BoxCollider* col = obj->GetComponent<BoxCollider>();
		if(col != NULL){
			XMLElement boxCol;
			boxCol.name = "BoxCollider";
			boxCol.attributes.push_back(XMLAttribute("position",EncodeVector3(col->position)));
			boxCol.attributes.push_back(XMLAttribute("size",EncodeVector3(col->size)));
			elem.children.push_back(boxCol);
		}
		SphereCollider* col2 = obj->GetComponent<SphereCollider>();
		if(col2 != NULL){
			XMLElement boxCol;
			boxCol.name = "SphereCollider";
			boxCol.attributes.push_back(XMLAttribute("position",EncodeVector3(col2->position)));
			boxCol.attributes.push_back(XMLAttribute("radius",to_string(col2->radius)));
			elem.children.push_back(boxCol);
		}

		for(auto iter = obj->components.begin(); iter != obj->components.end(); iter++){
			XMLElement newElem = (*iter)->Serialize();
			if(newElem.name != ""){
				elem.children.push_back(newElem);
			}
		}

		scene.children.push_back(elem);
	}

	doc.contents.push_back(scene);

	cout << "Saving to: " << fileName << endl;
	SaveXMLDoc(doc, fileName);
}

void Scene::LoadGuiElement(const XMLElement& elem){
	GuiElement* guiElem = AddGuiElement();
	int width=0,height=0;
	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "width"){
			width = atoi(iter->data.c_str());
		}
		else if (iter->name == "height"){
			height = atoi(iter->data.c_str());
		}
		else if (iter->name == "position"){
			guiElem->position = ParseVector2(iter->data);
		}
		else if (iter->name == "scale"){
			guiElem->scale = ParseVector2(iter->data);
		}
		else if (iter->name == "name"){
			guiElem->name = iter->data;
		}
	}

	guiElem->tex = new Texture(width, height);
}

void Scene::LoadGuiText(const XMLElement& elem){
	string fuv;
	Vector2 position;
	Vector2 scale;
	string name;
	string text;

	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "fuv"){
			fuv = iter->data;
		}
		else if(iter->name == "position"){
			position = ParseVector2(iter->data);
		}
		else if(iter->name == "scale"){
			scale = ParseVector2(iter->data);
		}
		else if (iter->name == "name"){
			name = iter->data;
		}
		else if (iter->name == "text"){
			text = iter->data;
		}
	}

	GuiText* guiText = new GuiText(&resources, fuv);
	guiText->position = position;
	guiText->scale = scale;
	guiText->name = name;
	guiText->text = text;

	guiElements.push_back(guiText);
}