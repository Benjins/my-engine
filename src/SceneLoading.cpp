#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"
#include "../header/int/GuiElement.h"
#include "../header/ext/simple-xml.h"

string EncodeVector3(Vector3 vec){
	return to_string(vec.x) + "," + to_string(vec.y)+ "," + to_string(vec.z);
}

string EncodeQuaternion(Quaternion quat){
	return to_string(quat.w) + "," + to_string(quat.x) + "," + to_string(quat.y)+ "," + to_string(quat.z);
}

Vector3 ParseVector3(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");
	float x = atof(parts[0].c_str());
	float y = atof(parts[1].c_str());
	float z = atof(parts[2].c_str());

	return Vector3(x,y,z);
}

Quaternion ParseQuaternion(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");

	float w = atof(parts[0].c_str());
	float x = atof(parts[1].c_str());
	float y = atof(parts[2].c_str());
	float z = atof(parts[3].c_str());

	return Quaternion(w,x,y,z);
}

void LoadMaterialXML(Scene* scene, XMLElement elem){
	string matName="", shaderName="",textureName="";
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
	}

	scene->resources.LoadMaterial(matName, shaderName, textureName);

}

void LoadGameObjectXML(Scene* scene, XMLElement elem){
	GameObject* go = new GameObject();
	go->scene = scene;

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
					GameObject* parent = scene->FindGameObject(attr.data);

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
			scene->camera = &(go->transform);
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
	}

	scene->AddObject(go);
}

void Scene::LoadScene(string fileName){

	RemoveAllObjects(); 

	XMLDocument doc;
	LoadXMLDoc(doc,fileName);

	for(auto iter = doc.contents.begin(); iter != doc.contents.end(); iter++){
		XMLElement topElem = *iter;

		if(topElem.name != "Scene"){
			continue;
		}
		else{
			for(auto iter2 = topElem.children.begin(); iter2 != topElem.children.end(); iter2++){
				XMLElement res = *iter2;

				if(res.name == "GameObject"){
					LoadGameObjectXML(this, res);
				}
				else if(res.name == "Material"){
					LoadMaterialXML(this, res);
				}
			}
		}
	}

	GuiElement* elem = AddGuiElement();
	elem->tex = new Texture(300,1);
	elem->position = Vector2(0.1,0.1);
	elem->scale = Vector2(0.2,0.02);
}


void Scene::SaveScene(string fileName){
	XMLDocument doc;

	XMLElement scene;
	scene.name = "Scene";
	scene.attributes.push_back(XMLAttribute("name", "my-engine"));

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
			material.attributes.push_back(XMLAttribute("shader",mat->shaderName));
			string textureName = (mat->mainTexture != NULL? mat->mainTexture->fileName : "");
			material.attributes.push_back(XMLAttribute("texture", textureName));
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

		scene.children.push_back(elem);
	}

	doc.contents.push_back(scene);

	cout << "Saving to: " << fileName << endl;
	SaveXMLDoc(doc, fileName);
}