#include "../headers/ObjWriter.h"

ObjWriter::ObjWriter()
{
}

ObjWriter::ObjWriter(string fileName, vector<float> curvaExterna, vector<float> curvaInterna, vector<float> altura)
{
	file = fileName;
	this->curvaExterna = curvaExterna;
	this->curvaInterna = curvaInterna;
	this->altura = altura;
}

void ObjWriter::createFile()
{
	obj.open(file);
	writeNewLine("mtllib pista.mtl");
}

void ObjWriter::writeNewLine(string text)
{
	obj << text << endl;
}

void ObjWriter::writeV()
{
	std::string text;
	int indexAltura = 0;
	//for curva externa
	for (int i = 0; i < curvaExterna.size(); i = i + 2) {
		float calcAltura = 0;
		text = "v " + to_string(curvaExterna[i]) + " " + to_string(altura[indexAltura++]) + " " + to_string(curvaExterna[i + 1]);
		writeNewLine(text);
	}
	writeNewLine("");

	indexAltura = 0;
	//for curva interna
	for (int i = 0; i < curvaInterna.size(); i = i + 2) {
		float calcAltura = 0;
		text = "v " + to_string(curvaInterna[i]) + " " + to_string(altura[indexAltura++]) + " " + to_string(curvaInterna[i + 1]);
		writeNewLine(text);
	}
	writeNewLine("");
}

void ObjWriter::writeVT()
{
	writeNewLine("vt 0.0 0.0");
	writeNewLine("vt 1.0 0.0");
	writeNewLine("vt 1.0 1.0");
	writeNewLine("vt 0.0 1.0");
	writeNewLine("");
}

void ObjWriter::writeVN()
{
	writeNewLine("vn 0.0 1.0 0.0");
	writeNewLine("");
}

void ObjWriter::writeGroup()
{
	writeNewLine("g pista");
	writeNewLine("usemtl pista");
}

void ObjWriter::writeFaces()
{
	//for externa e interna
	int size = curvaExterna.size() / 2;
	for (int i = 1; i < size; i++) {
		string face1 = to_string(i) + "/3/1 " +
			to_string(i + size) + "/1/1 " +
			to_string(i + size + 1) + "/2/1";
		string face2 = to_string(i + size + 1) + "/2/1 " +
			to_string(i + 1) + "/4/1 " +
			to_string(i) + "/3/1";
		writeNewLine("f " + face1);
		writeNewLine("f " + face2);
	}

}

void ObjWriter::MTLWriter()
{
	mtl.open("pista.mtl");
	mtl << "newmtl pista" << endl;
	mtl << "Ns 0.06" << endl;
	mtl << "Ni 1.00" << endl;
	mtl << "illum 2" << endl;
	mtl << "Ka  0.15 0.15 0.15" << endl;
	mtl << "Kd 0.50 0.50 0.50" << endl;
	mtl << "Ks 1.00 1.00 1.00" << endl;
	mtl << "map_Ka pista02.jpg" << endl;
	mtl << "map_Kd pista02.jpg" << endl;
	mtl.close();
}

ObjWriter::~ObjWriter()
{

}

void ObjWriter::write()
{
	createFile();
	writeV();
	writeVT();
	writeVN();
	writeGroup();
	writeFaces();
	obj.close();
}
