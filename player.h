#ifndef player_H
#define player_H

#include "glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>



class Player {
public:
	//std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	unsigned int vertexIndices, uvIndices, normalIndices;
	//std::vector< glm::vec3 > temp_vertices;
	glm::vec3 temp_vertices;
	//std::vector< glm::vec2 > temp_uvs;
	glm::vec2 temp_uvs;
	//std::vector< glm::vec3 > temp_normals;
	glm::vec3 temp_normals;

	FILE* file = fopen("human.obj", "r");
	
	/*if (file == NULL) 
	{
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
	}*/
};

#endif