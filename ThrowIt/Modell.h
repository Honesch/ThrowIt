#pragma once
#include <vector>
#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//testmaessig
#include <fstream>

#include "assimp/Importer.hpp"	
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "FrustumG.h"

//-.-.-..-.-.-.-.-.-.-..-


class Modell
{
public:
	Modell(glm::mat4& matrix, const char * modelpath);
	virtual ~Modell(void);
	virtual void draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullingEnabled);
	virtual void draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter);
	virtual void calcHitbox();
	virtual void updateHitbox();
	virtual AABox calcBox();

	float getYposAt(float x, float z);
	float getMaxX();
	float getMaxY();
	float getMaxZ();
	float getMinX();
	float getMinY();
	float getMinZ();

	AABox box;
	glm::mat4 mat;
	std::vector<glm::vec3> pos;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	aiMesh* mesh;

	float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;

	double hitboxRadius;//fuer Rechteckige ist die Funktion schon da aber zunaechst Runde
	glm::vec3 center;

	glm::vec3 diffuseMAT;
	glm::vec3 specularMAT;

private:
	
	GLuint vao;
	GLuint positionBuffer, indexBuffer, normalBuffer, uvBuffer;
	std::vector<GLuint> diffuseMaps;
	std::vector<GLuint> specularMaps;

	int positioncount;
	int indicescount;
	int normalscount;
	int uvcount;

};

