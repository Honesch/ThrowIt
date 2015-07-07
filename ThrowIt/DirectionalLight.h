#pragma once
#include <glm/glm.hpp>

class DirectionalLight
{
public:
	DirectionalLight(glm::vec3 position, glm::mat4 viewmatrix, glm::mat4 projectionmatrix);
	~DirectionalLight(void);
	glm::mat4 view;
	glm::mat4 projection; //entspricht kameraoeffnungsgroesse
	glm::vec3 pos;
};

