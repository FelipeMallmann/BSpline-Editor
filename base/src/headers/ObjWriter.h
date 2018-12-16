#pragma once

#include <string>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <vector>

using namespace std;

class ObjWriter
{
public:
	ofstream obj;
	ofstream mtl;
	string file;

	vector<float> curvaExterna;
	vector<float> curvaInterna;
	vector<float> altura;

	ObjWriter();
	ObjWriter(string fileName, vector<float> curvaExterna, vector<float> curvaInterna, vector<float> altura);


	void createFile();
	void writeNewLine(string text);
	void writeV();
	void writeVT();
	void writeVN();
	void writeGroup();
	void writeFaces();
	void write();
	void MTLWriter();
	void BSpline();
	~ObjWriter();
};


