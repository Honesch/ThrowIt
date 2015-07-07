#pragma once
#include "Modell.h"
#include "FrustumG.h"
class ThrowAble : public Modell
{
public:
	ThrowAble(glm::mat4& matrix, const char * modelpath, glm::vec3 dir, double f);
	virtual ~ThrowAble(void);

	virtual void update(float time_delta);
	virtual void draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullingEnabled);
	virtual void draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter);

	glm::vec3 direction;
	glm::vec3 movementVector;
	void setForce(double f);
	void setGravitation(double gravi);
	void calculateThrowParameters();

private:
	float force;
	float y;

	double g; //gravitation
};

