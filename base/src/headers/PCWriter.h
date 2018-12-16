#pragma once

#include <string>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <vector>

using namespace std;

class PCWriter
{
public:

	ofstream txt;

	PCWriter();
	PCWriter(vector<float> bs, vector<float> altura);
	~PCWriter();
};

