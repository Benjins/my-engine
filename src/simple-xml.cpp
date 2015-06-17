/*
Author: Benji Smith
Licensed under MIT License. See ../header/ext/simple-xml_LICENSE.txt for details
*/

#include "../header/ext/simple-xml.h"

XMLAttribute::XMLAttribute(string _name, string _data){
	name = _name;
	data = _data;
}

void XMLElement::Print() const{
	cout << name << " ";
	for(auto iter = attributes.begin(); iter != attributes.end(); iter++){
		iter->Print();
		cout << " ";
	}
	for(auto iter = children.begin(); iter != children.end(); iter++){
		cout << endl;
		iter->Print();
	}
}

string XMLElement::SaveElement(int depth/* = 0*/){
	string tabDepth = "";
	for(int i = 0; i < depth; i++){
		tabDepth += "    ";
	}

	string save = tabDepth + "<" + name;
	for(auto iter = attributes.begin(); iter != attributes.end(); iter++){
		save = save + " " + iter->ToString(); 
	}

	if(children.size() == 0){
		save += "/>";
		return save;
	}
	else{
		save += ">\n";
			
		for(auto iter = children.begin(); iter != children.end(); iter++){
			save += iter->SaveElement(depth + 1);
			save += "\n";
		}

		save = save + tabDepth + "</" + name + ">";
		return save;
	}
}

void XMLDocument::Print() const{
	cout << "Document: " << name << endl;
	for(auto iter = contents.begin(); iter != contents.end(); iter++){
		iter->Print();
		cout << endl;
	}
}

vector<string> Tokenize(const string& document){
	vector<string> tokens;
	string memoryString = "";
	bool inString = false;

	for(int i = 0; i < document.size(); i++){
		char character = document[i];

		if(inString && character != '\''  && character != '"'){
			memoryString += character;
		}
		else if(character == '<' || character == '>' || character == '/' || character == '='){
			if(memoryString != ""){
				tokens.push_back(memoryString);
				memoryString = "";
			}
			string str = "-";
			str[0] = character;
			tokens.push_back(str);
		}
		else if(character == '\'' || character == '"'){
			if(memoryString != "" || inString){
				tokens.push_back(memoryString);
				memoryString = "";
			}
			string str = "-";
			str[0] = character;
			//tokens.push_back(str);

			inString = !inString;
		}
		else if(character == ' ' || character == '\n' ||  character == '\t' || character == '\r'){
			if(memoryString != ""){
				tokens.push_back(memoryString);
				memoryString = "";
			}
		}
		else{
			memoryString += character;
		}
	}

	return tokens;
}

XMLDocument ParseTokens(vector<string> tokens){
	XMLDocument doc;

	Stack<string> tokenStack;
	Stack<XMLElement> elementStack;

	for(auto iter = tokens.begin(); iter != tokens.end(); iter++){
		string token = *iter;
		//cout << "Parsing token: |" << token << "|" << endl;
		if(token == "<"){
			tokenStack.Push(token);
		}
		else if(token == "/"){
			if(tokenStack.Top() == "<"){
				//Add the element to the doc
				XMLElement elem = elementStack.Pop();
				if(elementStack.count == 0){
					doc.contents.push_back(elem);
				}
				else{
					elementStack.Top().children.push_back(elem);
				}
				
				iter++;
				iter++;
			}
			else{
				tokenStack.Push(token);
			}
		}
		else if(token == ">"){
			bool startList = true;
			if(tokenStack.Top() == "/"){
				startList = false;
				tokenStack.Pop();
			}

			string currToken = tokenStack.Pop();
			XMLAttribute attr;
			attr.data = attr.name = "";
			bool gotData = false;
			while(currToken != "<"){
				if(currToken == "="){
					//do nothing
				}
				else if(!gotData){
					attr.data = currToken;
					gotData = true;
				}
				else{
					attr.name = currToken;
					elementStack.Top().attributes.push_back(attr);
					attr.data = attr.name = "";
					gotData = false;
				}
				currToken = tokenStack.Pop();
			}

			if(!startList){
				XMLElement elem = elementStack.Pop();
				if(elementStack.count == 0){
					doc.contents.push_back(elem);
				}
				else{
					elementStack.Top().children.push_back(elem);
				}
			}
		}
		else{
			if(tokenStack.Top() == "<"){
				XMLElement elem;
				elem.name = token;
				//cout << "Pushing to element stack: " << token << endl;
				elementStack.Push(elem);
			}
			//cout << "Pushing token: "<< token << endl;
			tokenStack.Push(token);
		}
	}

	return doc;
}

void SaveXMLDoc(XMLDocument& doc, string fileName){
	ofstream fileOut;
	fileOut.open(fileName.c_str(), ofstream::out);

	if(!fileOut.good()){
		cout << "Failed to open file " << fileName << " for saving.\n";
		return;
	}

	string docContents = "";

	for(auto iter = doc.contents.begin(); iter != doc.contents.end(); iter++){
		docContents += iter->SaveElement(0);
		docContents += "\n";
	}

	fileOut << docContents;
}


void LoadXMLDoc(XMLDocument& doc, string fileName){
	ifstream file;
	string fileContents;

	file.open(fileName.c_str());

	if(!file.good()){
		cout << "Error opening file: " << fileName << endl;
		return;
	}

	while(!file.eof()){
		string line;
		std::getline(file,line,'\n');
		fileContents = fileContents + line + "\n";
	}

	vector<string> tokens = Tokenize(fileContents);
	doc = ParseTokens(tokens);
}