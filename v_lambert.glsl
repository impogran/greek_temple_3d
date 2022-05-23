#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 ls1;
uniform vec4 ls2;
uniform vec4 color;


//Atrybuty
layout (location=0) in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout (location=1) in vec4 normal; //wektor normalny wierzcho³ka w przestrzeni modelu
layout (location=2) in vec2 texCoord; //wspó³rzêdne teksturowania

//Zmienne interpolowane
out vec4 l1;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 i_tc;


void main(void) {

    l1 = normalize(V * (ls1 - M * vertex));//znormalizowany wektor do œwiat³a w przestrzeni oka
    l2 = normalize(V * (ls2 - M * vertex));//znormalizowany wektor do œwiat³a w przestrzeni oka
    n = normalize(V * M * normal);//znormalizowany wektor normalny w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //Wektor do obserwatora w przestrzeni oka
    
    i_tc = texCoord;

    gl_Position=P*V*M*vertex;

}
