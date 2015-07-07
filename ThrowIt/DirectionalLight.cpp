#include "DirectionalLight.h"


DirectionalLight::DirectionalLight(glm::vec3 position, glm::mat4 viewmatrix, glm::mat4 projectionmatrix)
{
	view = viewmatrix;
	projection = projectionmatrix;
	pos = position;
}


DirectionalLight::~DirectionalLight(void)
{
}
