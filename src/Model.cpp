#include "../header/int/Model.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <time.h>

using std::ifstream; using std::cerr; using std::cout; using std::endl;

Face::Face(void){
	v0 = v1 = v2 = -1;
}

Face::Face(int _v0, int _v1, int _v2){
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
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
	
	string line = "";
	while(!importer.eof()){
		getline(importer, line, '\n');
		//cout << line << endl;
		if(line.size() < 2){
			continue;
		}
		else if(line[0] == 'v' && line[1] != 't'){
			//cout << "vertex line\n";
			vertices.push_back(ParseVertexLine(line));
		}
		else if(line[0] == 'f'){
			//cout << "face line\n";
			faces.push_back(ParseFaceLine(line));
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

Face ParseFaceLine(string line){
	vector<string> tokens = SplitStringByDelimiter(line, " ");
	
	if(tokens.size() != 4){
		return Face();
	}
	else{
		return Face(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()));
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