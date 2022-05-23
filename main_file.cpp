/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "camera.h"
#include "shader.h"
#include "player.h"
#include "OBJ_Loader.h"

#include <iostream>

std::vector<float> Pozycje;
std::vector<float> Normal;
std::vector<float> Textury;

float* PozycjeTab;
float* NormalTab;
float* TexturyTab;

float* PozycjeTab1;
float* NormalTab1;
float* TexturyTab1;

float* PozycjeTab2;
float* NormalTab2;
float* TexturyTab2;

int wierzcholki;
int wierzcholki1;
int wierzcholki2;

objl::Loader Loader;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//textura
GLuint texture;
GLuint texture1;
GLuint texture2;
ShaderProgram* spColored;
ShaderProgram* spTextured;
ShaderProgram* spWynikowy;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool tab[4] = {false, false, false, false};

// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;

float aspectRatio = 1;


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
		if (key == GLFW_KEY_W){
			tab[0] = true;
		}
		if (key == GLFW_KEY_S) {
			tab[1] = true;
		}
		if (key == GLFW_KEY_D) {
			tab[2] = true;
		}
		if (key == GLFW_KEY_A) {
			tab[3] = true;
		}
		if (key == GLFW_KEY_Q) {
			printf("X:%lf\n Y:%lf\n Z:%lf\n", camera.Position[0], camera.Position[1], camera.Position[2]);
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) {
			tab[0] = false;
		}
		if (key == GLFW_KEY_S) {
			tab[1] = false;
		}
		if (key == GLFW_KEY_D) {
			tab[2] = false;
		}
		if (key == GLFW_KEY_A) {
			tab[3] = false;
		}
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


GLuint readTexture(const char* filename) {
	GLuint texture;
	glActiveTexture(GL_TEXTURE0);

	//wczytranie do pamięci komputera
	std::vector<unsigned char> image; //alokuj wektor do wczytania obrazka
	unsigned width, height;
	//wczytanie obrazka
	unsigned error = lodepng::decode(image, width, height, filename);
	
	//import do pamieci karty graficznej
	glGenTextures(1, &texture); //inicjujemy jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, texture); //uaktywnij uchwyt
	//wczytaj obrazek do pamieci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	//Bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

void swiatlo(float x, float y, float z) {
	spWynikowy->use(); //Aktywuj program cieniujący
	glm::vec4 swiatlo = glm::vec4(x, y, z, 1);
	glUniform4fv(spWynikowy->u("ls1"), 1, glm::value_ptr(swiatlo));

	glm::vec4 swiatlo2 = glm::vec4(x - 23, y, z - 154, 1);
	glUniform4fv(spWynikowy->u("ls2"), 1, glm::value_ptr(swiatlo2));
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, keyCallback);
	texture = readTexture("piach.png");
	texture1 = readTexture("kolor_rzezb.png");
	texture2 = readTexture("ogniskokolor.png");

	bool loadout = Loader.LoadFile("budynek.obj");

	if (loadout)
	{
		wierzcholki = 0;
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{

			objl::Mesh curMesh = Loader.LoadedMeshes[i];
			wierzcholki += curMesh.Indices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				Pozycje.push_back(curMesh.Vertices[j].Position.X);
				Pozycje.push_back(curMesh.Vertices[j].Position.Y);
				Pozycje.push_back(curMesh.Vertices[j].Position.Z);
				Pozycje.push_back(1.0f);

				Normal.push_back(curMesh.Vertices[j].Normal.X);
				Normal.push_back(curMesh.Vertices[j].Normal.Y);
				Normal.push_back(curMesh.Vertices[j].Normal.Z);
				Normal.push_back(0.0f);

				Textury.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				Textury.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
			}
		}
	}

	PozycjeTab = new float[Pozycje.size()];
	NormalTab = new float[Normal.size()];
	TexturyTab = new float[Textury.size()];

	for (int i = 0; i < Pozycje.size(); i++) {
		PozycjeTab[i] = Pozycje[i];
	}

	for (int i = 0; i < Normal.size(); i++) {
		NormalTab[i] = Normal[i];
	}

	for (int i = 0; i < Textury.size(); i++) {
		TexturyTab[i] = Textury[i];
	}

	Pozycje.clear();
	Normal.clear();
	Textury.clear();

	bool loadout1 = Loader.LoadFile("rzezba2.obj");

	if (loadout1)
	{
		wierzcholki1 = 0;
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[i];
			wierzcholki1 += curMesh.Indices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				Pozycje.push_back(curMesh.Vertices[j].Position.X);
				Pozycje.push_back(curMesh.Vertices[j].Position.Y);
				Pozycje.push_back(curMesh.Vertices[j].Position.Z);
				Pozycje.push_back(1.0f);

				Normal.push_back(curMesh.Vertices[j].Normal.X);
				Normal.push_back(curMesh.Vertices[j].Normal.Y);
				Normal.push_back(curMesh.Vertices[j].Normal.Z);
				Normal.push_back(0.0f);

				Textury.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				Textury.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
			}
		}
	}

	PozycjeTab1 = new float[Pozycje.size()];
	NormalTab1 = new float[Normal.size()];
	TexturyTab1 = new float[Textury.size()];

	for (int i = 0; i < Pozycje.size(); i++) {
		PozycjeTab1[i] = Pozycje[i];
	}

	for (int i = 0; i < Normal.size(); i++) {
		NormalTab1[i] = Normal[i];
	}

	for (int i = 0; i < Textury.size(); i++) {
		TexturyTab1[i] = Textury[i];
	}

	Pozycje.clear();
	Normal.clear();
	Textury.clear();

	bool loadout2 = Loader.LoadFile("ognisko.obj");

	if (loadout2)
	{
		wierzcholki2 = 0;
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{

			objl::Mesh curMesh = Loader.LoadedMeshes[i];
			wierzcholki2 += curMesh.Indices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				Pozycje.push_back(curMesh.Vertices[j].Position.X);
				Pozycje.push_back(curMesh.Vertices[j].Position.Y);
				Pozycje.push_back(curMesh.Vertices[j].Position.Z);
				Pozycje.push_back(1.0f);

				Normal.push_back(curMesh.Vertices[j].Normal.X);
				Normal.push_back(curMesh.Vertices[j].Normal.Y);
				Normal.push_back(curMesh.Vertices[j].Normal.Z);
				Normal.push_back(0.0f);

				Textury.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				Textury.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
			}
		}
	}

	PozycjeTab2 = new float[Pozycje.size()];
	NormalTab2 = new float[Normal.size()];
	TexturyTab2 = new float[Textury.size()];

	for (int i = 0; i < Pozycje.size(); i++) {
		PozycjeTab2[i] = Pozycje[i];
	}

	for (int i = 0; i < Normal.size(); i++) {
		NormalTab2[i] = Normal[i];
	}

	for (int i = 0; i < Textury.size(); i++) {
		TexturyTab2[i] = Textury[i];
	}

	Pozycje.clear();
	Normal.clear();
	Textury.clear();
	spColored = new ShaderProgram("v_colored.glsl", NULL, "f_colored.glsl");
	spTextured = new ShaderProgram("v_textured.glsl", NULL, "f_textured.glsl");
	spWynikowy = new ShaderProgram("v_lambert.glsl", NULL, "f_lambert.glsl");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	glDeleteTextures(1, &texture);
	delete spColored;
	delete spTextured;
	delete spWynikowy;
}


void budynek(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
	spWynikowy->use(); //Aktywuj program cieniujący

	M = glm::scale(M, glm::vec3(0.8f, 0.8f, 0.8f));

	glUniformMatrix4fv(spWynikowy->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spWynikowy->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spWynikowy->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu


	glEnableVertexAttribArray(spWynikowy->a("vertex"));
	glVertexAttribPointer(spWynikowy->a("vertex"), 4, GL_FLOAT, false, 0, PozycjeTab);

	glEnableVertexAttribArray(spWynikowy->a("normal"));
	glVertexAttribPointer(spWynikowy->a("normal"), 4, GL_FLOAT, false, 0, NormalTab);

	glEnableVertexAttribArray(spWynikowy->a("texCoord"));
	glVertexAttribPointer(spWynikowy->a("texCoord"), 2, GL_FLOAT, false, 0, TexturyTab);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(spWynikowy->u("texture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, wierzcholki);

	glDisableVertexAttribArray(spWynikowy->a("vertex"));
	glDisableVertexAttribArray(spWynikowy->a("normal"));
	glDisableVertexAttribArray(spWynikowy->a("texCoord"));
}

void rzezba(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
	spWynikowy->use(); //Aktywuj program cieniujący

	M = glm::translate(M, glm::vec3(-45.0f, -10.0f, 64.0f));
	M = glm::scale(M, glm::vec3(15.0f, 15.0f, 15.0f));
	M = glm::rotate(M, 250.0f * 3.14f / 180.0f, glm::vec3(0.0f, 1.0f, .0f));

	glUniformMatrix4fv(spWynikowy->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spWynikowy->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spWynikowy->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu


	glEnableVertexAttribArray(spWynikowy->a("vertex"));
	glVertexAttribPointer(spWynikowy->a("vertex"), 4, GL_FLOAT, false, 0, PozycjeTab1);

	glEnableVertexAttribArray(spWynikowy->a("normal"));
	glVertexAttribPointer(spWynikowy->a("normal"), 4, GL_FLOAT, false, 0, NormalTab1);

	glEnableVertexAttribArray(spWynikowy->a("texCoord"));
	glVertexAttribPointer(spWynikowy->a("texCoord"), 2, GL_FLOAT, false, 0, TexturyTab1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glUniform1i(spWynikowy->u("texture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, wierzcholki1);

	glDisableVertexAttribArray(spWynikowy->a("vertex"));
	glDisableVertexAttribArray(spWynikowy->a("normal"));
	glDisableVertexAttribArray(spWynikowy->a("texCoord"));
}

void ognisko(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
	spWynikowy->use(); //Aktywuj program cieniujący

	M = glm::translate(M, glm::vec3(19.0f, -10.0f, 68.0f));
	M = glm::scale(M, glm::vec3(4.0f, 4.0f, 4.0f));

	glUniformMatrix4fv(spWynikowy->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spWynikowy->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spWynikowy->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu


	glEnableVertexAttribArray(spWynikowy->a("vertex"));
	glVertexAttribPointer(spWynikowy->a("vertex"), 4, GL_FLOAT, false, 0, PozycjeTab2);

	glEnableVertexAttribArray(spWynikowy->a("normal"));
	glVertexAttribPointer(spWynikowy->a("normal"), 4, GL_FLOAT, false, 0, NormalTab2);

	glEnableVertexAttribArray(spWynikowy->a("texCoord"));
	glVertexAttribPointer(spWynikowy->a("texCoord"), 2, GL_FLOAT, false, 0, TexturyTab2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(spWynikowy->u("texture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, wierzcholki2);

	glDisableVertexAttribArray(spWynikowy->a("vertex"));
	glDisableVertexAttribArray(spWynikowy->a("normal"));
	glDisableVertexAttribArray(spWynikowy->a("texCoord"));
}

void ognisko1(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
	spWynikowy->use(); //Aktywuj program cieniujący

	M = glm::translate(M, glm::vec3(-4.0f, -10.0f, -86.0f));
	M = glm::scale(M, glm::vec3(4.0f, 4.0f, 4.0f));

	glUniformMatrix4fv(spWynikowy->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spWynikowy->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spWynikowy->u("M"), 1, false, glm::value_ptr(M));


	glEnableVertexAttribArray(spWynikowy->a("vertex"));
	glVertexAttribPointer(spWynikowy->a("vertex"), 4, GL_FLOAT, false, 0, PozycjeTab2);

	glEnableVertexAttribArray(spWynikowy->a("normal"));
	glVertexAttribPointer(spWynikowy->a("normal"), 4, GL_FLOAT, false, 0, NormalTab2);

	glEnableVertexAttribArray(spWynikowy->a("texCoord"));
	glVertexAttribPointer(spWynikowy->a("texCoord"), 2, GL_FLOAT, false, 0, TexturyTab2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(spWynikowy->u("texture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, wierzcholki2);

	glDisableVertexAttribArray(spWynikowy->a("vertex"));
	glDisableVertexAttribArray(spWynikowy->a("normal"));
	glDisableVertexAttribArray(spWynikowy->a("texCoord"));
}


void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysujący obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.ProcessKeyboard(tab, deltaTime);

	glm::mat4 P = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 V = camera.GetViewMatrix();

	glm::mat4 Model = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową

	budynek(P, V, Model);
	rzezba(P, V, Model);
	ognisko(P, V, Model);
	ognisko1(P, V, Model);
	swiatlo(19.0, 0.0, 68.0);

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

int main(void)
{
	

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1024, 1024, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące
	
	//Główna pętla
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		// per-frame time logic
		double currentFrame = glfwGetTime() + lastFrame;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glfwSetTime(0); //Zeruj timer
		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}

