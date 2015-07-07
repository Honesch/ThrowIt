#include "Partikelsystem.h"

#define NUM_PARTICLE_ATTRIBUTES 6
#define MAX_PARTICLES_ON_SCENE 10000 //max 100000

#define PARTICLE_TYPE_GENERATOR 0
#define PARTICLE_TYPE_NORMAL 1

Partikelsystem::Partikelsystem(void)
{
	initalize();
}


Partikelsystem::~Partikelsystem(void)
{
	/*glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);

	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(zShaders);
	free(light);
	glDeleteBuffers(1, &fb);
	glDeleteTextures(1, &ztex);
	
	*/

	glDeleteProgram(updateprogram);
}

bool Partikelsystem::initalize()
{
	//Ausgabeparameter des Transform feedback systems
	const char* varyings[] =  
	{ 
      "vColorOut",
      "vVelocityOut",
	  "vPositionOut",  
      "fLifeTimeOut", 
      "fSizeOut", 
      "iTypeOut", 
	}; 

	Shaderloader* loader = new Shaderloader();
	updateprogram = loader->LoadTransformFeedbackShaders("../Shader/particleUpdateVertexShader.vert", "../Shader/particleUpdateGeometryShader.geo", varyings, NUM_PARTICLE_ATTRIBUTES);

	rederprogram = loader->LoadShaders("../Shader/particleRenderVertexShader.vert","../Shader/particleRenderFragmentShader.frag", "../Shader/particleRenderGeometryShader.geo");
	delete loader; loader = nullptr;


	glGenTransformFeedbacks(1, &uiTransformFeedbackBuffer); 
	glGenQueries(1, &uiQuery);	//dieser Query "zaehlt" fuer uns wie viele Partikel erzeugt wurden, damit wir nicht manuell mitzaehlen muessen

	glGenBuffers(2, uiParticleBuffer);	//2 buffer da diese nach den updaten geswappt werden
	glGenVertexArrays(2, uiVAO);			//deswegen auch 2 vaos

	CParticle partInitialization; 
	partInitialization.iType = PARTICLE_TYPE_GENERATOR; 
	
	for(int i = 0; i<2; i++) 
	{    
		glBindVertexArray(uiVAO[i]); 
		glBindBuffer(GL_ARRAY_BUFFER, uiParticleBuffer[i]); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(CParticle)*MAX_PARTICLES_ON_SCENE, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(CParticle), &partInitialization); //das initiale Partikel gleich in den Buffer laden

		for(int j = 0; j < NUM_PARTICLE_ATTRIBUTES; j++)
		{
			glEnableVertexAttribArray(j); //alle Attribute aktivieren
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CParticle), (const GLvoid*)0); // Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CParticle), (const GLvoid*)12); // Velocity
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(CParticle), (const GLvoid*)24); // Position
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(CParticle), (const GLvoid*)36); // Lifetime
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(CParticle), (const GLvoid*)40); // Size
		glVertexAttribPointer(5, 1, GL_INT,   GL_FALSE, sizeof(CParticle), (const GLvoid*)44); // Type
	} 
	iCurReadBuffer = 0;	//der aktuelle Buffer ist der 1. Buffer
	iNumParticles = 1;	// ein Partikel ist im Buffer

	Texture texloader;
	uiTexture = texloader.loadTexture("../Models/particle.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	return true; 
}

float Partikelsystem::grandf(float fMin, float fAdd)
{
	float fRandom = float(rand()%(RAND_MAX+1))/float(RAND_MAX);
	return fMin+fAdd*fRandom;
}

void Partikelsystem::update(float delta_time)
{
	glUseProgram(updateprogram);

	auto deltaTimeloc = glGetUniformLocation(updateprogram, "fTimePassed");
	auto posloc = glGetUniformLocation(updateprogram,"vGenPosition");
	auto minVeloloc = glGetUniformLocation(updateprogram, "vGenVelocityMin");
	auto rangeVeloloc = glGetUniformLocation(updateprogram, "vGenVelocityRange");
	auto colorloc = glGetUniformLocation(updateprogram, "vGenColor");
	auto gravityloc = glGetUniformLocation(updateprogram, "vGenGravityVector");
	auto minLifeloc = glGetUniformLocation(updateprogram, "fGenLifeMin");
	auto rangeLifeloc = glGetUniformLocation(updateprogram, "fGenLifeRange");
	auto sizeloc = glGetUniformLocation(updateprogram, "fGenSize");
	auto generateNumloc = glGetUniformLocation(updateprogram, "iNumToGenerate");
	auto randomSeedloc = glGetUniformLocation(updateprogram, "vRandomSeed");
	
	glUniform1f(deltaTimeloc, (float) delta_time);
	glUniform3f(posloc, vGenPosition.x, vGenPosition.y, vGenPosition.z);
	glUniform3f(minVeloloc, vGenVelocityMin.x, vGenVelocityMin.y, vGenVelocityMin.z);
	glUniform3f(rangeVeloloc, vGenVelocityRange.x, vGenVelocityRange.y, vGenVelocityRange.z);
	glUniform3f(colorloc, vGenColor.x, vGenColor.y, vGenColor.z);
	glUniform3f(gravityloc, vGenGravityVector.x, vGenGravityVector.y, vGenGravityVector.z);

	glUniform1f(minLifeloc, (float) fGenLifeMin);
	glUniform1f(rangeLifeloc, (float) fGenLifeRange);

	glUniform1f(sizeloc, (float) fGenSize);   
	glUniform1i(generateNumloc, 0);   
	
	fElapsedTime += delta_time; 

	if(fElapsedTime > fNextGenerationTime) 
	{ 
		glUniform1i(generateNumloc, iNumToGenerate); 
		fElapsedTime -= fNextGenerationTime; 

		glm::vec3 vRandomSeed = glm::vec3(grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f));
		glUniform3f(randomSeedloc, vRandomSeed.x, vRandomSeed.y, vRandomSeed.z);
	} 

	glEnable(GL_RASTERIZER_DISCARD); 
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, uiTransformFeedbackBuffer); 

	glBindVertexArray(uiVAO[iCurReadBuffer]); 
	glEnableVertexAttribArray(1); // Re-enable velocity

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, uiParticleBuffer[1-iCurReadBuffer]); 

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, uiQuery); 
	glBeginTransformFeedback(GL_POINTS); 

	glDrawArrays(GL_POINTS, 0, iNumParticles); 

	glEndTransformFeedback(); 

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN); 
	glGetQueryObjectiv(uiQuery, GL_QUERY_RESULT, &iNumParticles); 

	/*glFlush();
	CParticle feedback[100];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback); //debugging
	*/

	iCurReadBuffer = 1-iCurReadBuffer; 

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0); //unbind
}

void Partikelsystem::draw(float delta_time)
{
	update(delta_time);
	 
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	glDepthMask(0); 

	glDisable(GL_RASTERIZER_DISCARD); 
   
	glUseProgram(rederprogram);

	auto mprojloc = glGetUniformLocation(rederprogram, "matrices.mProj");
	auto mviewloc = glGetUniformLocation(rederprogram,"matrices.mView");
	auto quad1loc = glGetUniformLocation(rederprogram, "vQuad1");
	auto quad2loc = glGetUniformLocation(rederprogram, "vQuad2");
	auto samplerloc = glGetUniformLocation(rederprogram, "gSampler");

	glUniformMatrix4fv(mprojloc, 1, GL_FALSE, glm::value_ptr(matProjection));
	glUniformMatrix4fv(mviewloc, 1, GL_FALSE, glm::value_ptr(matView));
	glUniform3f(quad1loc, vQuad1.x, vQuad1.y, vQuad1.z);
   	glUniform3f(quad2loc, vQuad2.x, vQuad2.y, vQuad2.z);

	glActiveTexture(GL_TEXTURE0 + 20);
	glBindTexture(GL_TEXTURE_2D, uiTexture); 
	glUniform1i(samplerloc, 20);

	glBindVertexArray(uiVAO[iCurReadBuffer]); 
	glDisableVertexAttribArray(1); // Disable velocity, because we don't need it for rendering

	glDrawArrays(GL_POINTS, 0, iNumParticles); 

	glDepthMask(1);    
	glDisable(GL_BLEND); 
}

void Partikelsystem::setMatrices(glm::mat4* a_matProjection, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUpVector)
{
	matProjection = *a_matProjection; 
	matView = glm::lookAt(vEye, vView, vUpVector); 
	
	vView = vView-vEye; 
	vView = glm::normalize(vView); 
	vQuad1 = glm::cross(vView, vUpVector); 

	vQuad1 = glm::normalize(vQuad1); 
	vQuad2 = glm::cross(vView, vQuad1); 
	vQuad2 = glm::normalize(vQuad2); 
}

void Partikelsystem::setProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector,
		glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate)
{
	vGenPosition = a_vGenPosition;
	vGenVelocityMin = a_vGenVelocityMin;
	vGenVelocityRange = a_vGenVelocityMax - a_vGenVelocityMin;

	vGenGravityVector = a_vGenGravityVector;
	vGenColor = a_vGenColor;
	fGenSize = a_fGenSize;

	fGenLifeMin = a_fGenLifeMin;
	fGenLifeRange = a_fGenLifeMax - a_fGenLifeMin;

	fNextGenerationTime = fEvery;
	fElapsedTime = 0.8f;

	iNumToGenerate = a_iNumToGenerate;
}

int Partikelsystem::getNumParticles()
{
	return iNumParticles;
}