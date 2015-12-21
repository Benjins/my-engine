/*
Author: Benji Smith
Licensed under MIT License. See License.txt for details
*/

#ifndef SIMPLE_XML_H
#define SIMPLE_XML_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using std::string; using std::vector; using std::ifstream;
using std::cout; using std::endl; using std::ofstream;
using std::map;

#define MAX_STACK_SIZE 8192

template<typename T>
struct Stack{
	T* values;//[MAX_STACK_SIZE];
	int count;

	Stack(){
		count = 0;
		values = new T[MAX_STACK_SIZE];
	}

	void Push(const T& newValue){ 
		values[count] = newValue;
		count++;
	}

	T Pop(){
		count--;
		return values[count];
	}

	T& Top(){
		return values[count-1];
	}

	~Stack(){
		delete[] values;
	}
};

struct XMLAttribute{
	string name;
	string data;

	XMLAttribute()
	: name(), data(){
	}

	XMLAttribute(const string& _name, const string& _data)
		: name(_name), data(_data){
	}

	XMLAttribute(const string&& _name, const string&& _data)
		: name(_name), data(_data){	
	}

	void Print() const{
		cout << name << ":" << data;
	}

	string ToString(){
		return name + "='" + data + "'";
	}
};

struct XMLElement{
	string name;
	vector<XMLAttribute> attributes;
	map<string, string> attributeMap;
	vector<XMLElement> children;

	void AddAttribute(const string& name, const string& data){
		attributes.emplace_back(name, data);
		attributeMap.insert(std::make_pair(name, data));
	}

	void AddAttribute(const string&& name, const string&& data){
		attributes.emplace_back(name, data);
		attributeMap.insert(std::make_pair(name, data));
	}

	void Print() const;

	string SaveElement(string tabbing = "");
};

struct XMLDocument{
	string name;
	vector<XMLElement> contents;

	void Print() const;
};

struct Token{
	const char* start;
	int length;

	bool operator==(const string& param){
		if(param.length() == length){
			return memcmp(start, param.c_str(), length) == 0;
		}

		return false;
	}

	bool operator!=(const string& param){
		if(param.length() == length){
			return memcmp(start, param.c_str(), length) != 0;
		}

		return true;
	}

	bool operator==(const char* param){
		for(int i = 0; i < length; i++){
			if(start[i] != param[i]){
				return false;
			}
		}

		return param[length] == '\0';
	}

	bool operator!=(const char* param){
		for(int i = 0; i < length; i++){
			if(start[i] != param[i]){
				return true;
			}
		}

		return param[length] != '\0';
	}

	string ToString(){
		if(length ==0){
			return "";
		}
		string str = string(start, length);
		return str;
	}
};

vector<Token> Tokenize(const string& document);

void ParseTokens(vector<Token>& tokens, XMLDocument& outDoc);

void SaveXMLDoc(XMLDocument& doc, string fileName);

void LoadXMLDoc(XMLDocument& doc, string fileName);


#endif