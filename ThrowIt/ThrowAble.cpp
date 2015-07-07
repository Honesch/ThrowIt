#include "ThrowAble.h"


ThrowAble::ThrowAble(glm::mat4& matrix, const char * modelpath, glm::vec3 dir, double f) : Modell(matrix,modelpath),
	direction(dir), force(f)
{
	g = 9.81;
	
}

ThrowAble::~ThrowAble(void)
{
}
void ThrowAble::setForce(double f)
{
	force = f;
}
void ThrowAble::setGravitation(double gravi)
{
	g = gravi;
}

void ThrowAble::update(float time_delta)
{  
	glm::vec3 trans = glm::vec3(Modell::mat[3].x, Modell::mat[3].y, Modell::mat[3].z); //position des Objekts

	movementVector[1] -= g*0.0004;
	glm::vec3 frameindependentMovement(movementVector);
	frameindependentMovement *= time_delta;

	glm::mat4 bewegung = glm::translate(trans + movementVector);	
	Modell::mat = bewegung;
}

void ThrowAble::draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullingEnabled)
{
	Modell::draw(handle, textureI, minificationFilter, magnificationFilter, frustum, cullingEnabled);
}

void ThrowAble::draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter)
{
	Modell::draw(handle, textureI, minificationFilter, magnificationFilter);
}

void ThrowAble::calculateThrowParameters(){
	movementVector = direction * force * 0.05f;
}


