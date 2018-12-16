#include "../headers/PCWriter.h"


PCWriter::PCWriter()
{
}

PCWriter::PCWriter(vector<float> bs, vector<float> altura)
{
	float x, y, z;
	int iAltura = 0;
	txt.open("pontos.txt");
	for (int i = 0; i < bs.size(); i = i + 2) {
		x = bs[i];
		z = bs[i + 1];
		y = altura[iAltura++];
		txt << to_string(x) << " " << to_string(y) << " " << to_string(z) << endl;
	}

	txt.close();
}


PCWriter::~PCWriter()
{
}