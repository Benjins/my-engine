#include "../header/int/Material.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Scene.h"
#include "../header/ext/simple-xml.h"

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

void LoadGameObjectXML(Scene* scene, XMLElement elem){
	GameObject* go = new GameObject();
	go->scene = scene;

	for(auto iter = elem.children.begin(); iter != elem.children.end(); iter++){
		XMLElement child = *iter;
		if(child.name == "Transform"){
			SC_Transform trans;
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;

				if(attr.name == "position"){
					trans.position = ParseVector3(attr.data);
				}
				else if(attr.name == "rotation"){
					trans.rotation = ParseQuaternion(attr.data);
				}
				else if(attr.name == "scale"){
					trans.scale = ParseVector3(attr.data);
				}
			}
		}
		else if(child.name == "Material"){
			string shaderName="", textureName="";
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "shader"){
					shaderName = attr.data;
				}
				else if(attr.name == "texture"){
					textureName = attr.data;
				}
			}

			go->AddMaterial(shaderName,textureName,true,false);
		}
		else if(child.name == "Mesh"){
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "source"){
					go->AddMesh(attr.data);
				}
			}
		}
		else if(child.name == "BoxCollider"){
			//stufff
		}
	}

	scene->AddObject(go);
}

void Scene::LoadScene(string fileName){
	XMLDocument doc;

	cout << "About to load doc.\n";

	LoadXMLDoc(doc,fileName);

	cout << "Loaded document" << endl;

	for(auto iter = doc.contents.begin(); iter != doc.contents.end(); iter++){
		XMLElement topElem = *iter;

		if(topElem.name != "Scene"){
			continue;
		}
		else{
			cout << "Parsing scene" << endl;
			for(auto iter2 = topElem.children.begin(); iter2 != topElem.children.end(); iter2++){
				XMLElement res = *iter2;

				if(res.name == "GameObject"){
					LoadGameObjectXML(this, res);
				}
			}
		}
	}
}