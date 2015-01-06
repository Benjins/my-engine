#include "../header/int/Model.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <time.h>

using std::ifstream; using std::cerr; using std::cout; using std::endl;

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
}

Vertex::Vertex(Vector3 _position){
	position = _position;
}

Model::Model(){
}

int Model::GLVertexCount() const{
	return faces.size() * 3;
}

Model::Model(const Model& model){
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
	ImportFromOBJ(fileName);
}

void Model::ImportFromOBJ(string fileName){
	srand(time(NULL));
	ifstream importer;
	importer.open(fileName);
	if(!importer.good()){
		cerr << "Error: Could not open OBJ file" << fileName << ": File does not exist.\n";
		return;
	}

	name = fileName;
	
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