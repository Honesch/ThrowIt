#pragma once

#include <vector>
#include <glew.h>
#include <glm/glm.hpp>

class Waterplane
{
public:
	Waterplane(int length, int heigth, GLuint watershade);
	~Waterplane(void);

	void setboudaries(int length, int heigth);
	void draw();
	glm::mat4 getmat();

private:
	glm::mat4 mat;
	std::vector<glm::vec3> pos;

	int indexcount;
	int vertexcount;
	int uvCoordcount;

	GLuint vao;
	GLuint positionBuffer, indexBuffer, normalBuffer, uvBuffer;
};

