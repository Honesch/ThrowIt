#include "Waterplane.h"


Waterplane::Waterplane(int length, int heigth, GLuint watershade)
{
	mat = glm::mat4(1);

	indexcount = 2 * 3;
	vertexcount = 4 * 3;
	uvCoordcount = 4 * 2;

	std::vector<unsigned int> indices;
	std::vector<glm::vec2> uvs;

	setboudaries(length, heigth);

	unsigned int index = 0;
	indices.push_back(index);
	index = 1;
	indices.push_back(index);
	index = 2;
	indices.push_back(index);
	index = 1;
	indices.push_back(index);
	index = 3;
	indices.push_back(index);
	index = 2;
	indices.push_back(index);

	glm::vec2 uv;
	uv = glm::vec2(1,0);
	uvs.push_back(uv);
	uv = glm::vec2(1,1);
	uvs.push_back(uv);
	uv = glm::vec2(0,1);
	uvs.push_back(uv);
	uv = glm::vec2(0,0);
	uvs.push_back(uv);

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexcount * sizeof(float), &pos[0], GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexcount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvCoordcount * sizeof(float), &uvs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(watershade);

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void Waterplane::setboudaries(int length, int heigth)
{
	pos.clear();
	glm::vec3 position;
	position = glm::vec3((-1)*length,0, heigth);
	pos.push_back(position);
	position = glm::vec3(length,0, heigth);
	pos.push_back(position);
	position = glm::vec3((-1)*length,0, (-1)*heigth);
	pos.push_back(position);
	position = glm::vec3(length,0, (-1)*heigth);
	pos.push_back(position);
	
}

Waterplane::~Waterplane(void)
{
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &uvBuffer);

	glDeleteVertexArrays(1, &vao);
}

void Waterplane::draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::mat4 Waterplane::getmat()
{
	return mat;
}