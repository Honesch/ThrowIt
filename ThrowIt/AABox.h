#pragma once
#include <glm\glm.hpp>
class AABox 
{

public:

	glm::vec3 corner;
	float x,y,z;


	AABox::AABox( glm::vec3 &corner, float x, float y, float z);
	AABox::AABox(void);
	AABox::~AABox();

	void AABox::setBox( glm::vec3 &corner, float x, float y, float z);

	// for use in frustum computations
	glm::vec3 AABox::getVertexP(glm::vec3 &normal);
	glm::vec3 AABox::getVertexN(glm::vec3 &normal);


};


