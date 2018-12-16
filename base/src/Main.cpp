#include <stdio.h>
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <vector>
#include <math.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/Shader.h"
#include "headers/ObjWriter.h"
#include "headers/PCWriter.h"

#define PI 3.14159265

using namespace std;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void keyButtonCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void resize(GLFWwindow* window, int width, int height);
void updateBuffer(GLuint vao, GLuint vbo, GLsizeiptr size, const GLvoid* data, GLenum usage);
void calcularPontosSpline();
void calcularAltura();
void desenhar(Shader shader);
void desenhar2(Shader shader);
void generateCurves();
int findNearestPoint(double mx, double my);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool isEditing = true;
bool isSaved = false;

// Vectors
vector<float> pc, pcC;
vector<float> bs, ci, ce, cc;

int nIndex = 0; // index para o array de cores
vector<float> altura;

// VAOs VBOs
GLuint vao1, vao2, vao3, vao4;
GLuint vbo1, vbo2, vbo3, vbo4, vbo5;

// Projection
glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);

int main() {

	GLFWwindow* window = NULL;
	glfwInit();

	window = glfwCreateWindow(
		SCR_WIDTH, SCR_HEIGHT, "B-Spline Curve Editor", NULL, NULL
	);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed no init GLEW." << std::endl;
		return EXIT_FAILURE;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Shader
	Shader shader("res/shaders/basic.vs", "res/shaders/basic.fs");
	shader.setMat4("projection", projection);

	Shader shader2("res/shaders/basic2.vs", "res/shaders/basic2.fs");
	shader2.setMat4("projection", projection);

	// Ponto Controle
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);
	glGenBuffers(1, &vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, pc.size() * sizeof(float), pc.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &vbo5);
	glBindBuffer(GL_ARRAY_BUFFER, vbo5);
	glBufferData(GL_ARRAY_BUFFER, pcC.size() * sizeof(float), pcC.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Curva Meio
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, bs.size() * sizeof(float), bs.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// Curva Interna
	glGenVertexArrays(1, &vao3);
	glBindVertexArray(vao3);
	glGenBuffers(1, &vbo3);
	glBindBuffer(GL_ARRAY_BUFFER, vbo3);
	glBufferData(GL_ARRAY_BUFFER, ci.size() * sizeof(float), ci.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// Curva Externa
	glGenVertexArrays(1, &vao4);
	glBindVertexArray(vao4);
	glGenBuffers(1, &vbo4);
	glBindBuffer(GL_ARRAY_BUFFER, vbo4);
	glBufferData(GL_ARRAY_BUFFER, ce.size() * sizeof(float), ce.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	std::cout << "Editor de Curvas B-Spline Iniciado" << std::endl;
	std::cout << "S - Salvar a curva" << std::endl;
	std::cout << "S - Alterna o modo de edicao (Editar Curva / Editar Altura (Pontos)" << std::endl;
	std::cout << "+ - Aumenta a altura do ponto selecionado" << std::endl;
	std::cout << "- - Diminui a altura do ponto selecionado" << std::endl;
	std::cout << "----------------------------" << std::endl;

	// Callbacks
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyButtonCallback);

	while (!glfwWindowShouldClose(window)) {

		glfwSetWindowSizeCallback(window, resize);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		desenhar(shader);

		shader2.use();
		desenhar2(shader2);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	if (isSaved) {
		ObjWriter *obj = new ObjWriter("pista.obj", ce, ci, cc);
		obj->write();
		obj->MTLWriter();

		PCWriter *pcwriter = new PCWriter(bs, cc);
	}
	glfwTerminate();
	return 0;
}

void keyButtonCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	else if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
		isEditing = !isEditing;
		std::cout << "Modo de edicao alterado para: " << (isEditing ? "Curvas" : "Pontos") << std::endl;
	}

	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		isSaved = true;

		// save obj
		generateCurves();

		updateBuffer(vao2, vbo2, bs.size() * sizeof(float), bs.data(), GL_STATIC_DRAW);
		updateBuffer(vao3, vbo3, ci.size() * sizeof(float), ci.data(), GL_STATIC_DRAW);
		updateBuffer(vao4, vbo4, ce.size() * sizeof(float), ce.data(), GL_STATIC_DRAW);
		calcularAltura();
		std::cout << "Curva salva com Sucesso." << std::endl;

		std::cout << bs.size() << std::endl;
		std::cout << cc.size() << std::endl;
	}

	else if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE) {
		if (altura[nIndex / 3] < 1.0) {
			altura[nIndex / 3] = altura[nIndex / 3] + 0.1;
			pcC[nIndex] = pcC[nIndex] - 0.1;
			pcC[nIndex + 1] = pcC[nIndex + 1] - 0.1;
			pcC[nIndex + 2] = pcC[nIndex + 2] + 0.1;

			updateBuffer(vao1, vbo5, pcC.size() * sizeof(float), pcC.data(), GL_STATIC_DRAW);

			std::cout << "Altura atual do ponto[" << nIndex / 3 << "]: " << altura[nIndex / 3] << std::endl;
		}
		else {
			std::cout << "Altura ja esta no maximo!" << std::endl;
		}
	}

	else if (key == GLFW_KEY_MINUS && action == GLFW_RELEASE) {
		if (altura[nIndex / 3] > 0.11) { // bugado
			altura[nIndex / 3] = altura[nIndex / 3] - 0.1;
			pcC[nIndex] = pcC[nIndex] + 0.1;
			pcC[nIndex + 1] = pcC[nIndex + 1] - 0.1;
			pcC[nIndex + 2] = pcC[nIndex + 2] - 0.1;

			updateBuffer(vao1, vbo5, pcC.size() * sizeof(float), pcC.data(), GL_STATIC_DRAW);

			std::cout << "Altura atual do ponto[" << nIndex / 3 << "]: " << altura[nIndex / 3] << std::endl;
		}
		else {
			std::cout << "Altura ja esta no minimo!" << std::endl;
		}
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (!isSaved) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

			double xpos, ypos = 0;
			glfwGetCursorPos(window, &xpos, &ypos);

			if (isEditing) {

				// positions
				pc.push_back(xpos);
				pc.push_back(ypos);

				// r, g, b
				pcC.push_back(0.5f);
				pcC.push_back(0.5f);
				pcC.push_back(0.5f);

				// altura
				altura.push_back(0.5);

				updateBuffer(vao1, vbo1, pc.size() * sizeof(float), pc.data(), GL_STATIC_DRAW);
				updateBuffer(vao1, vbo5, pcC.size() * sizeof(float), pcC.data(), GL_STATIC_DRAW);

				if (pc.size() >= 8) {
					calcularPontosSpline();
					updateBuffer(vao2, vbo2, bs.size() * sizeof(float), bs.data(), GL_STATIC_DRAW);
				}
			}
			else {
				nIndex = findNearestPoint(xpos, ypos);

				// nIndex retorna o indice do pc.X mais proximo, mas como o pcC tem 3 floats ao invés de 2
				// tem que ajustar-lo para representar sempre o pcC.X 
				nIndex = nIndex + nIndex / 2;

				//std::cout << ">>" << nIndex << std::endl;
			}
		}
	}
}

void desenhar(Shader shader) {
	shader.setMat4("projection", projection);

	/*
	if (pc.size() > 0) {
		glBindVertexArray(vao1);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, pc.size() / 2);
		glDisable(GL_POINT_SMOOTH);
		if (pc.size() > 2) {
			glDrawArrays(GL_LINE_STRIP, 0, pc.size() / 2);
		}
	}*/

	if (bs.size() > 0 && ci.size() == 0) {
		glBindVertexArray(vao2);
		glDrawArrays(GL_LINE_STRIP, 0, bs.size() / 2);
	}

	if (ci.size() > 0) {
		glBindVertexArray(vao3);
		glDrawArrays(GL_LINE_STRIP, 0, ci.size() / 2);
	}

	if (ce.size() > 0) {
		glBindVertexArray(vao4);
		glDrawArrays(GL_LINE_STRIP, 0, ce.size() / 2);
	}
}

void desenhar2(Shader shader) {
	shader.setMat4("projection", projection);

	if (pc.size() > 0) {
		glBindVertexArray(vao1);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, pc.size() / 2);
		glDisable(GL_POINT_SMOOTH);
		if (pc.size() > 2 && !isEditing) {
			glDrawArrays(GL_LINE_STRIP, 0, pc.size() / 2);
		}
	}
}

void calcularPontosSpline() {

	bs.clear();
	int n = pc.size();
	for (int i = 0; i < n; i = i + 2) {
		for (float t = 0; t <= 1; t += 0.05) {

			float x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*pc[i] +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*pc[(i + 2) % n] +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*pc[(i + 4) % n] +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*pc[(i + 6) % n]) / 6);

			float y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*pc[(i + 1) % n] +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*pc[(i + 3) % n] +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*pc[(i + 5) % n] +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*pc[(i + 7) % n]) / 6);

			bs.push_back(x);
			bs.push_back(y);
		}
	}

	// Last Point = First Point
	bs.push_back(bs[0]);
	bs.push_back(bs[1]);
}

void calcularAltura() {

	int n = altura.size();
	for (int i = 0; i < n; i++)
		altura[i] = altura[i] * 200;

	cc.clear();

	for (int i = 0; i < n; i++) {
		for (float t = 0; t <= 1; t += 0.05) {

			float x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*altura[i] +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*altura[(i + 1) % n] +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*altura[(i + 2) % n] +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*altura[(i + 3) % n]) / 6);

			cc.push_back(x);
		}
	}

	// Last Point = First Point
	cc.push_back(cc[0]);
}


void updateBuffer(GLuint vao, GLuint vbo, GLsizeiptr size, const GLvoid* data, GLenum usage) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void generateCurves() {
	for (int i = 0; i < bs.size() - 2; i = i + 2) {
		float ax = bs[i];
		float ay = bs[i + 1];
		float bx = bs[i + 2];
		float by = bs[i + 3];

		float w = bx - ax;
		float h = by - ay;
		float m = 20;

		float theta = atan(h / w);
		float alfa = 0;

		// Internal
		if (w < 0)
			alfa = theta + (PI / 2);
		else
			alfa = theta - (PI / 2);

		float cx = cos(alfa) * m + ax;
		float cy = sin(alfa) * m + ay;

		ci.push_back(cx);
		ci.push_back(cy);

		// External
		if (w < 0)
			alfa = theta - (PI / 2);
		else
			alfa = theta + (PI / 2);

		cx = cos(alfa) * m + ax;
		cy = sin(alfa) * m + ay;

		ce.push_back(cx);
		ce.push_back(cy);
	}

	// Last Point = First Point
	if (ci.size() > 0) {
		ci.push_back(ci[0]);
		ci.push_back(ci[1]);
	}
	if (ce.size() > 0) {
		ce.push_back(ce[0]);
		ce.push_back(ce[1]);
	}
}

int findNearestPoint(double mx, double my) {
	int r = 0;
	float a = SCR_WIDTH;

	for (int i = 0; i < pc.size(); i = i + 2) {
		float d = sqrt((pow((mx - pc[i]), 2)) + (pow((my - pc[i + 1]), 2))); // euclidiane distance

		if (d < a) {
			r = i;
			a = d;
		}
	}
	return r;
}