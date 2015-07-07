#pragma once

//basiert mit Anpassungen auf http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=26

#include <glm\glm.hpp>
#include <glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaderloader.h"
#include "Texture.h"

class CParticle                                                                         
{ 
public: 
	glm::vec3 vColor;
	glm::vec3 vVelocity;		//Geschwindigkeit und Richtung		
	glm::vec3 vPosition;		//position
	float fLifeTime;			
	float fSize;				//falls unterschiedliche partikelgroessen gewuenscht werden. sonst unnoetig
	int iType;				//1..normales partikel, 0..generatorpartikel
};


class Partikelsystem
{
public:
	Partikelsystem(void);
	~Partikelsystem(void); 

	void draw(float delta_time); 

	void clearAll(); 
	bool releaseParticleSystem(); 

	int getNumParticles();
	void setProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector,
		glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate); 

	void setMatrices(glm::mat4* a_matProjection, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUpVector); 

private:
	bool initalize();
	void update(float delta_time); 
	float grandf(float fMin, float fAdd);


	bool bInitialized; 

	GLuint uiTransformFeedbackBuffer; 

	GLuint uiParticleBuffer[2]; 
	GLuint uiVAO[2]; 

	GLuint uiQuery; 
	GLuint uiTexture; 

	unsigned int iCurReadBuffer; 
	int iNumParticles; 

	glm::mat4 matProjection, matView; 
	glm::vec3 vQuad1, vQuad2; 

	float fElapsedTime; 
	float fNextGenerationTime; 

	glm::vec3 vGenPosition; 
	glm::vec3 vGenVelocityMin, vGenVelocityRange; 
	glm::vec3 vGenGravityVector; 
	glm::vec3 vGenColor; 

	float fGenLifeMin, fGenLifeRange; 
	float fGenSize; 

	int iNumToGenerate; 

	//GLuint shVertexRender, shGeomRender, shFragRender; 
	//GLuint shVertexUpdate, shGeomUpdate, shFragUpdate; 
	GLuint rederprogram; 
	GLuint updateprogram; 
};


