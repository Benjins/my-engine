#include "../header/int/Model.h"
#include "../header/int/Scene.h"
#include "../header/int/Armature.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"
#include "../header/ext/simple-xml.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <map>
#include <time.h>

using std::ifstream; using std::cerr; using std::cout; using std::endl; using std::map;

Face::Face(void){
	v0 = v1 = v2 = -1;
	uv0 = uv1 = uv2 = Vector2(0,0);
}

Face::Face(int _v0, int _v1, int _v2){
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
	uv0 = uv1 = uv2 = Vector2(0,0);
}

Vertex::Vertex(void){
	position = Vector3(0,0,0);
	color = Vector3(0,0,0);
	numBones = 0;
}

Vertex::Vertex(Vector3 _position){
	position = _position;
	numBones = 0;
}

Model::Model(){
	name="";
	fileName="";
	vertices = vector<Vertex>();
	faces = vector<Face>();
	armature = nullptr;
}

int Model::GLVertexCount() const{
	return faces.size() * 3;
}

Model::Model(const Model& model){
	name = model.name;
	fileName = model.fileName;

	armature = model.armature;

	//Copy the vertices
	vertices.resize(model.vertices.size());
	for(int i = 0; i < model.vertices.size(); i++){
		vertices[i] = model.vertices[i];
	}
	
	//Copy the faces
	faces.resize(model.faces.size());
	for(int i = 0; i < model.faces.size(); i++){
		faces[i] = model.faces[i];
	}
}

Model::Model(string fileName){
	this->name="";
	this->fileName="";
	vertices = vector<Vertex>();
	faces = vector<Face>();

	armature = nullptr;

	string fileExt = fileName.substr(fileName.length() - 4, 4);
	if(fileExt == ".obj"){
		ImportFromOBJ(fileName);
	}
	else if(fileExt == ".dae"){
		ImportFromCollada(fileName);
	}
	else{
		cout << "\n\nError: unrecognised file format for file: '" << fileName << "'\n";
	}
}

Model::~Model(){
	delete armature;
}

void Model::CalculateNormals(){
	for(int i = 0; i < vertices.size(); i++){
		Vector3 totalNormal = Vector3(0,0,0);
		int totalNormalCount = 0;
		for(int j = 0; j < faces.size(); j++){
			Face face = faces[j];
			if(face.v0 == i || face.v1 == i || face.v2 == i){
				Vector3 v0 = vertices[face.v0].position;
				Vector3 v1 = vertices[face.v1].position;
				Vector3 v2 = vertices[face.v2].position;

				Vector3 edge1 = v1 - v0;
				Vector3 edge2 = v2 - v0;

				Vector3 normal = CrossProduct(edge1, edge2).Normalized();
				totalNormal = totalNormal + normal;
				totalNormalCount++;
			}
		}

		Vector3 finalNormal = totalNormal / totalNormalCount;
		vertices[i].normal = finalNormal;
	}
}

void Model::CalculateTangents(){
	for(int i = 0; i < vertices.size(); i++){
		Vector3 totalTangent = Vector3(0,0,0);
		int totalTangentCount = 0;

		for(int j = 0; j < faces.size(); j++){
			Face face = faces[j];
			if(face.v0 == i || face.v1 == i || face.v2 == i){
				Vector3 v0 = vertices[face.v0].position;
				Vector3 v1 = vertices[face.v1].position;
				Vector3 v2 = vertices[face.v2].position;

				
				Vector3 edge1 = (v1 - v0).Normalized();
				Vector3 edge2 = (v2 - v0).Normalized();

				Vector2 uvEdge1 = (face.uv1 - face.uv0).Normalized();
				Vector2 uvEdge2 = (face.uv2 - face.uv0).Normalized();

				//uvEdge1.x * a + uvEdge2.x * b = 1;
				//uvEdge1.y * a + uvEdge2.y * b = 0;
				//
				//uvEdge1.x * aOverB * b + uvEdge2.x * b = 1;
				//1 / (uvEdge1.x * aOverB + uvEdge2.x) = b;

				float a,b;

				if(uvEdge2.y != 0){
					float bOverA = -uvEdge1.y/uvEdge2.y;
					a = 1 / (uvEdge2.x * bOverA + uvEdge1.x);
					b = bOverA * a;
				}
				else if(uvEdge1.y != 0){
					float aOverB = -uvEdge2.y/uvEdge1.y;
					b = 1 / (uvEdge1.x * aOverB + uvEdge2.x);
					a  = aOverB * b;
				}
				else{
					a = 0.7f;
					b = 0.7f;
				}


				

				Vector3 tangent = edge1 * a + edge2 * b;
				totalTangent = totalTangent + tangent;
				totalTangentCount++;
			}
		}

		Vector3 finalTangent = totalTangent / totalTangentCount;

		vertices[i].tangent = finalTangent;
	}
}

void Model::ImportFromOBJ(const string& fileName){
	ifstream importer;
	importer.open(fileName);
	if(!importer.good()){
		cerr << "Error: Could not open OBJ file: " << fileName << ". File does not exist.\n";
		return;
	}

	this->fileName = fileName;
	this->name = fileName;
	
	string line = "";

	vector<Vector2> uvs;

	while(!importer.eof()){
		getline(importer, line, '\n');
		//cout << line << endl;
		if(line.size() < 2){
			continue;
		}
		else if(line[0] == 'v' && line[1] == 't'){
			//cout << "vertex line\n";
			uvs.push_back(ParseUVLine(line));
		}
		else if(line[0] == 'v'){
			vertices.push_back(ParseVertexLine(line));
		}
		else if(line[0] == 'f'){
			//cout << "face line\n";
			faces.push_back(ParseFaceLine(line, uvs));
		}
	}

	CalculateNormals();
	CalculateTangents();

	if(fileName == "data/test_2.obj"){
		cout << "\nFaking armature info.\n";
		for(int i = 0; i < vertices.size(); i++){
			vertices[i].AddBone(i % 2, 0.5f);
			vertices[i].AddBone(i % 3, 0.5f);
		}

		armature = Scene::getInstance().testArmature;
		cout << "Adding armature, it's " << (armature == nullptr ? "null" : "not null") << endl;
	}
}

void ParseColladaSourceElement(const XMLElement& element, map<string, vector<string>>& sources){
	string id = element.attributeMap.find("id")->second;
	
	const XMLElement& floatArray = element.children[0];
	int count = atoi(floatArray.attributeMap.find("count")->second.c_str());

	if(floatArray.children.size() != 1){
		cout << "\n\nError: the element with name '" << element.name << "' either has an extra tag, or is missing its data.\n";
		return;
	}

	const XMLElement& plainTextInfo = floatArray.children[0];
	
	if(plainTextInfo.name != "__plaintext__" || plainTextInfo.attributes.size() != 1){
		cout << "\n\nError: the element with name '" << element.name << "' is missing its data.\n";
		return;
	}

	const string& floatDataStr = plainTextInfo.attributes[0].data;
	vector<string> nameData;
	nameData.reserve(count);
	stringstream floatConversion;
	floatConversion << floatDataStr;

	if(count <= 0){
		cout << "\n\nError: the element with name '" << element.name << "' has an improper count: '" << count << "'.\n";
		return; 
	}

	for(int i = 0; i < count; i++){
		string datum;
		floatConversion >> datum;
		nameData.push_back(datum);
	}

	sources.insert(std::make_pair(id, nameData));
}

void ParseColladaSourceElement(const XMLElement& element, map<string, vector<float>>& sources){
	string id = element.attributeMap.find("id")->second;
	
	const XMLElement& floatArray = element.children[0];
	int count = atoi(floatArray.attributeMap.find("count")->second.c_str());

	if(floatArray.children.size() != 1){
		cout << "\n\nError: the element with name '" << element.name << "' either has an extra tag, or is missing its data.\n";
		return;
	}

	const XMLElement& plainTextInfo = floatArray.children[0];
	
	if(plainTextInfo.name != "__plaintext__" || plainTextInfo.attributes.size() != 1){
		cout << "\n\nError: the element with name '" << element.name << "' is missing its data.\n";
		return;
	}

	const string& floatDataStr = plainTextInfo.attributes[0].data;
	vector<float> floatData;
	floatData.reserve(count);
	stringstream floatConversion;
	floatConversion << floatDataStr;

	if(count <= 0){
		cout << "\n\nError: the element with name '" << element.name << "' has an improper count: '" << count << "'.\n";
		return; 
	}

	for(int i = 0; i < count; i++){
		float datum=0;
		floatConversion >> datum;
		floatData.push_back(datum);
	}

	sources.insert(std::make_pair(id, floatData));
}

void Model::ImportFromCollada(const string& fileName){

	this->fileName = fileName;
	this->name = fileName;

	XMLDocument colladaDoc;
	LoadXMLDoc(colladaDoc, fileName);

	if(colladaDoc.contents.size() != 1){
		cout << "\n\nError: File '" << fileName << "' is not a properly formatted COLLADA file.\n";
		return;
	}

	const XMLElement& colladaElem = colladaDoc.contents[0];

	Mat4x4 conv;
	conv.SetRow(0, Vector4(1, 0,  0, 0));
	conv.SetRow(1, Vector4(0, 0,  1, 0));
	conv.SetRow(2, Vector4(0, -1,  0, 0));
	conv.SetRow(3, Vector4(0, 0,  0, 1));

	Mat4x4 revConv;
	revConv.SetRow(0, Vector4(1,  0, 0, 0));
	revConv.SetRow(1, Vector4(0,  0, -1, 0));
	revConv.SetRow(2, Vector4(0, 1, 0, 0));
	revConv.SetRow(3, Vector4(0,  0, 0, 1));

	//is this needed
	vector<Mat4x4> invBindPoses;

	for(const XMLElement& colladaChild : colladaElem.children){
		if(colladaChild.name == "library_geometries"){
			if(colladaChild.children.size() != 1 || colladaChild.children[0].children.size() != 1){
				cout << "\n\nError: File '" << fileName << "' has an invalid number of geometries or meshes: " << colladaChild.children.size() << ".\n";
				return;
			}

			const XMLElement& mesh = colladaChild.children[0].children[0];

			map<string, vector<float>> sources;

			for(const XMLElement& meshChild : mesh.children){
				if(meshChild.name == "source"){
					ParseColladaSourceElement(meshChild, sources);
				}
				else if(meshChild.name == "vertices"){
					if(meshChild.children.size() == 0){
						cout << "\n\nError: File '" << fileName << "' has a mesh with an improperly formatted 'vertices' tag.";
						return;
					}
					
					const XMLElement& vertInput = meshChild.children[0];

					if(vertInput.name != "input" || vertInput.attributes.size() != 2){
						cout << "\n\nError: File '" << fileName << "' has a mesh with an improperly formatted 'vertices' tag.";
						return;
					}

					//TODO: some thigns do this out of order.  Like UV's.
					if(vertInput.attributeMap.find("semantic")->second != "POSITION"){
						cout << "\n\nError: File '" << fileName << "' has a mesh with vertices with an improperly formatted 'input' tag.";
						return;
					}

					const string& vertPosSourceName= vertInput.attributeMap.find("source")->second;

					if(vertPosSourceName.size() == 0 || vertPosSourceName[0] != '#'){
						cout << "\n\nError: File '" << fileName << "' has an 'input' tag with a badly formatted source link.";
						return;
					}

					string vertPosLinkName = vertPosSourceName.substr(1);
					auto vertSource = sources.find(vertPosLinkName);
					if(vertSource == sources.end()){
						cout << "\n\nError: File '" << fileName << "' has an 'input' tag with an incorrect source link.";
						return;
					}

					const vector<float>& vertData = vertSource->second;
					vertices.resize(vertData.size() / 3);
					for(int i = 0; i < vertices.size(); i++){
						Vertex v;
						v.position =  conv * Vector3(vertData[3*i], vertData[3*i+1], vertData[3*i+2]);
						vertices[i] = v;
					}
				}
				else if(meshChild.name == "polylist"){
					auto iter = meshChild.attributeMap.find("count");
					if(iter == meshChild.attributeMap.end()){
						cout << "\n\nError: count does not exist.\n";
					}

					int count = atoi(iter->second.c_str());
					vector<int> vCounts;
					vCounts.resize(count);

					int vertOffset=0;
					int uvOffset=0;
					int vertAttrCount=0;
					vector<float>* uvArray=nullptr;
					for(const auto& input : meshChild.children){
						if(input.name == "input"){
							vertAttrCount++;
						}

						auto semantic = input.attributeMap.find("semantic");
						if(semantic != input.attributeMap.end() && semantic->second == "VERTEX"){
							vertOffset = atoi(input.attributeMap.find("offset")->second.c_str());
						}
						else if(semantic != input.attributeMap.end() && semantic->second == "TEXCOORD"){
							uvOffset = atoi(input.attributeMap.find("offset")->second.c_str());
							uvArray = &(sources.find(input.attributeMap.find("source")->second.substr(1))->second);
						}
						else if(input.name == "vcount"){
							const string& vCountData = input.children[0].attributeMap.find("val")->second;
							stringstream vCount;
							vCount << vCountData;
							for(int i = 0; i < count; i++){
								vCount >> vCounts[i];
							}
						}
						else if(input.name == "p"){
							const string& polyDataStr = input.children[0].attributes[0].data;
							stringstream polyData;
							polyData << polyDataStr;
							int index = 0;

							vector<int> polyDataBuffer;
							for(int i = 0; i < vCounts.size(); i++){
								int vCount = vCounts[i];
								polyDataBuffer.resize(vCount * vertAttrCount);
								for(int i = 0; i < vCount * vertAttrCount; i++){
									polyData >> polyDataBuffer[i];
								}

								int posIndices[3];
								int uvIndices[3];
								for(int j = 0; j < 3; j++){
									posIndices[j] = polyDataBuffer[vertAttrCount * j + vertOffset];
									uvIndices[j]  = polyDataBuffer[vertAttrCount * j + uvOffset];
								}

								Face face;
								face.v0 = posIndices[0];
								face.v1 = posIndices[1];
								face.v2 = posIndices[2];
								if(uvArray != nullptr){
									face.uv0 = Vector2(uvArray->at(2*uvIndices[0]),uvArray->at(2*uvIndices[0]+1));
									face.uv1 = Vector2(uvArray->at(2*uvIndices[1]),uvArray->at(2*uvIndices[1]+1));
									face.uv2 = Vector2(uvArray->at(2*uvIndices[2]),uvArray->at(2*uvIndices[2]+1));
								}
								faces.push_back(face);

								index += vCount * vertAttrCount;
							}
						}
					}
				}
			}

		}
		else if(colladaChild.name == "library_controllers"){
			if(colladaChild.children.size() == 0){
				break;
			}

			if(colladaChild.children[0].children.size() == 0){
				cout << "\n\nError: Collada File '" << fileName << "' has an improperly formatted controller.\n";
				break;
			}

			armature = new Armature();
			
			const XMLElement& skinElem = colladaChild.children[0].children[0];
			map<string, vector<float>> sources;
			map<string, vector<string>> nameSources;

			for(const XMLElement& source: skinElem.children){
				if(source.name == "source" && source.children.size() > 0){
					if(source.children[0].name == "Name_array"){
						ParseColladaSourceElement(source, nameSources);
						const vector<string>& boneNames = nameSources.find(source.attributeMap.find("id")->second)->second;
						for(const string& boneName : boneNames){
							BoneTransform* bone = armature->AddBone(nullptr);
							bone->name= boneName;
						}
					}
					else if(source.children[0].name == "float_array"){
						ParseColladaSourceElement(source, sources);
					}
				}
				else if(source.name == "joints"){
					for(const XMLElement& input : source.children){
						if(input.attributeMap.find("semantic")->second == "INV_BIND_MATRIX"){
							const string sourceLink = input.attributeMap.find("source")->second;

							const vector<float>& invBindData = sources.find(sourceLink.substr(1))->second;

							invBindPoses.resize(invBindData.size() / 16);
							for(int i = 0; i < invBindPoses.size(); i++){
								memcpy(invBindPoses[i].m, &(invBindData.data()[16 * i]), 16*sizeof(float));
							}
						}
					}
				}
				else if(source.name == "vertex_weights"){
					int count = atoi(source.attributeMap.find("count")->second.c_str());
					vector<int> vCounts;
					vCounts.resize(count);

					vector<float>* weightsArray=nullptr;

					int inputCount=0;
					int jointOffset=0;
					int weightOffset=0;
					for(const XMLElement& skinChild : source.children){
						if(skinChild.name == "input"){
							auto iter = skinChild.attributeMap.find("semantic");
							if(iter->second == "JOINT"){
								jointOffset = atoi(skinChild.attributeMap.find("offset")->second.c_str());
								inputCount++;
							}
							else if(iter->second == "WEIGHT"){
								weightOffset = atoi(skinChild.attributeMap.find("offset")->second.c_str());
								const string& sourceLink = skinChild.attributeMap.find("source")->second;
								weightsArray = &(sources.find(sourceLink.substr(1))->second);
								inputCount++;
							}
						}
						else if(skinChild.name == "vcount"){
							const string& plaintextVal = skinChild.children[0].attributeMap.find("val")->second;
							stringstream vCountParser;
							vCountParser << plaintextVal;
							for(int i = 0; i < vCounts.size(); i++){
								vCountParser >> vCounts[i];
							}
						}
						else if(skinChild.name == "v"){
							const string& plaintextVal = skinChild.children[0].attributeMap.find("val")->second;
							stringstream indexBuferParser;
							indexBuferParser << plaintextVal;

							int index = 0;
							for(int i = 0; i < vCounts.size(); i++){
								int vCount = vCounts[i];

								vector<int> indicesBufer;
								indicesBufer.resize(vCount * inputCount);

								for(int j = 0; j < indicesBufer.size(); j++){
									indexBuferParser >> indicesBufer[j];
								}

								if(vCount > 4){
									cout << "\nError: COLLADA file '" << fileName << "' has more than 4 bones per vertex.  There's a limit of 4.\n";
								}

								for(int j = 0; j < 4; j++){
									if(j < vCount){
										int boneIndexBufferIndex = (j*inputCount) + jointOffset;
										vertices[i].boneIndices[j] = indicesBufer[boneIndexBufferIndex];

										int boneWeightIndexIndex = (j*inputCount) + weightOffset;
										int boneWeightIndex = indicesBufer[boneWeightIndexIndex];
										vertices[i].boneWeights[j] = weightsArray->at(boneWeightIndex);
									}
									else{
										vertices[i].boneWeights[j] = 0;
									}
								}

								vertices[i].numBones = vCount;

								index += inputCount * vCount;
							}
						}
					}

					int yy = 0;
				}
			}
		}
		else if(colladaChild.name == "library_visual_scenes"){
			if(colladaChild.children.size() != 1){
				cout << "\n\nError: file '" << fileName << "' has the wrong number of visual scenes.\n";
				continue;
			}
			const XMLElement& visualScene = colladaChild.children[0];

			const XMLElement* searchStack[MAX_BONE_COUNT];
			int searchCount = 0;
			searchStack[searchCount++] = &(visualScene.children[0]);
			while(searchCount > 0){
				const XMLElement* node = searchStack[--searchCount];
				for(const XMLElement& child : node->children){
					if(child.name == "matrix"){
						const string& matrixPlaintext = child.children[0].attributeMap.find("val")->second;
						stringstream matrixParser;
						matrixParser << matrixPlaintext;
						BoneTransform* bone = armature->GetBoneByName(node->attributeMap.find("id")->second);

						Mat4x4 mat;
						for(int x = 0; x < 4; x++){
							for(int y = 0; y < 4; y++){
								matrixParser >> mat.m[x][y];
							}
						}

						size_t boneIdx = ((size_t)bone - (size_t)armature->bones) / sizeof(BoneTransform);

						mat = conv * mat * revConv;
						armature->bindPoses[boneIdx] = conv * invBindPoses[boneIdx] * revConv;

						//Get quaternion from matrix
						Vector4 rotX = mat * Vector4(X_AXIS, 0);
						Vector3 rotatedX = Vector3(rotX.w, rotX.x, rotX.y).Normalized();
						
						Vector3 vec1 = CrossProduct(X_AXIS, rotatedX);
						float sinAngle1 = vec1.Magnitude();
						float angle1 = asinf(sinAngle1);
						float halfAngle1 = angle1/2;
						float cosHalfAngle1 = cosf(halfAngle1);
						float sinHalfAngle1 = sinf(halfAngle1);
						vec1 = vec1.Normalized() / sinHalfAngle1;
						float w1 = cosHalfAngle1;
						Quaternion initialQuat = Quaternion(w1, vec1.x, vec1.y, vec1.z).Normalized();

						Vector4 rotY = mat * Vector4(Rotate(Y_AXIS, initialQuat), 0);
						Vector3 rotatedY = Vector3(rotY.w, rotY.x, rotY.y).Normalized();

						Vector3 vec2 = CrossProduct(Y_AXIS, rotatedY);
						float sinAngle2 = vec2.Magnitude();
						float angle2 = asinf(sinAngle2);
						float halfAngle2 = angle2/2;
						float cosHalfAngle2 = cosf(halfAngle2);
						float sinHalfAngle2 = sinf(halfAngle2);
						vec2 = vec2.Normalized() / sinHalfAngle2;
						float w2 = cosHalfAngle2;
						Quaternion secondQuat  = Quaternion(w2, vec2.x, vec2.y, vec2.z).Normalized();

						Quaternion finalQuat = secondQuat * initialQuat;
						bone->position = mat * Vector3(0,0,0);
						bone->rotation = finalQuat;

					}
					else if(child.name == "node"){
						BoneTransform* boneChild = armature->GetBoneByName(child.attributeMap.find("id")->second);
						BoneTransform* boneParent = armature->GetBoneByName(node->attributeMap.find("id")->second);

						boneChild->SetParent(boneParent);

						searchStack[searchCount++] = &child;
					}
				}
			}
		}
	}

	BoneTransform* root = &armature->bones[0];
	if(root != nullptr){
		for(SC_Transform* child : root->children){
			BoneTransform* boneChild = static_cast<BoneTransform*>(child);

			for(SC_Transform* grandChild : boneChild->children){
				BoneTransform* boneGrandChild = static_cast<BoneTransform*>(grandChild);
				int qqq = 0;
			}
		}
	}

	if(armature != nullptr){
		armature->model = this;
	}

	CalculateNormals();
	CalculateTangents();


	if(armature != nullptr){
		Scene::getInstance().testArmature = armature;
	}
}

Vertex ParseVertexLine(string line){
	vector<string> tokens = SplitStringByDelimiter(line, " ");
	
	if(tokens.size() != 4){
		return Vertex();
	}
	else{
		Vector3 pos = Vector3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
		return Vertex(pos);
	}
}

Vector2 ParseUVLine(string line){
	vector<string> tokens = SplitStringByDelimiter(line, " ");

	if(tokens.size() != 3){
		return Vector2();
	}
	else{
		return Vector2(atof(tokens[1].c_str()), atof(tokens[2].c_str()));
	}
}

Face ParseFaceLine(string line, const vector<Vector2>& uvs){
	vector<string> tokens = SplitStringByDelimiter(line, " ");
	
	if(tokens.size() != 4){
		return Face();
	}
	else{
		vector<string> faceDataSplit0 = SplitStringByDelimiter(tokens[1],"/");
		if(faceDataSplit0.size() > 1){
			vector<string> faceDataSplit1 = SplitStringByDelimiter(tokens[2],"/");
			vector<string> faceDataSplit2 = SplitStringByDelimiter(tokens[3],"/");

			Face face = Face(atoi(faceDataSplit0[0].c_str())-1, 
							 atoi(faceDataSplit1[0].c_str())-1, 
							 atoi(faceDataSplit2[0].c_str())-1);

			face.uv0 = uvs[atoi(faceDataSplit0[1].c_str())-1];
			face.uv1 = uvs[atoi(faceDataSplit1[1].c_str())-1];
			face.uv2 = uvs[atoi(faceDataSplit2[1].c_str())-1];

			return face;
		}
		Face face = Face(atoi(tokens[1].c_str())-1, 
						 atoi(tokens[2].c_str())-1, 
						 atoi(tokens[3].c_str())-1);
		return face;
	}
}

vector<string> SplitStringByDelimiter(string searchIn, string delimiter){
	vector<string> splits;

	string searchString = searchIn;

	size_t occurence = 0;
	string token;
	while(occurence != string::npos){
		occurence = searchString.find(delimiter);
		token = searchString.substr(0,occurence);
		splits.push_back(token);
		searchString.erase(0, occurence + delimiter.length());
	}

	return splits;
}