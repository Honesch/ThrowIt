#pragma once

#include "skybox.h"
#include <glew.h>
#include <glm\glm.hpp>

/*-----------------------------------------------

Name:	loadSkybox

Params:	paths of skybox pictures

Result:	Loads skybox and creates VAO and VBO for it.

/*---------------------------------------------*/

void CSkybox::loadSkybox(float _dimension ,std::string a_sDirectory,std::string a_sFront,std::string a_sBack,std::string a_sLeft, std::string a_sRight, std::string a_sTop, std::string a_sBottom)
{
	dimension = _dimension;
	tTextures[0].loadTexture2D(a_sDirectory+a_sFront);
	tTextures[1].loadTexture2D(a_sDirectory+a_sBack);
	tTextures[2].loadTexture2D(a_sDirectory+a_sLeft);
	tTextures[3].loadTexture2D(a_sDirectory+a_sRight);
	tTextures[4].loadTexture2D(a_sDirectory+a_sTop);
	tTextures[5].loadTexture2D(a_sDirectory+a_sBottom);

	sDirectory = a_sDirectory;

	sFront = a_sFront;
	sBack = a_sBack;
	sLeft = a_sLeft;
	sRight = a_sRight;
	sTop = a_sTop;
	sBottom = a_sBottom;

	//FOR(i, 6)
	for (int i=0; i<6; i++)
	{
		tTextures[i].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		tTextures[i].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tTextures[i].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	vboRenderData.createVBO();
	vboRenderData.bindVBO();

	glm::vec3 vSkyBoxVertices[24] = 
	{
		// Front face
		glm::vec3(dimension, dimension, dimension), glm::vec3(dimension, -dimension, dimension), glm::vec3(-dimension, dimension, dimension), glm::vec3(-dimension, -dimension, dimension),
		// Back face
		glm::vec3(-dimension, dimension, -dimension), glm::vec3(-dimension, -dimension, -dimension), glm::vec3(dimension, dimension, -dimension), glm::vec3(dimension, -dimension, -dimension),
		// Left face
		glm::vec3(-dimension, dimension, dimension), glm::vec3(-dimension, -dimension, dimension), glm::vec3(-dimension, dimension, -dimension), glm::vec3(-dimension, -dimension, -dimension),
		// Right face
		glm::vec3(dimension, dimension, -dimension), glm::vec3(dimension, -dimension, -dimension), glm::vec3(dimension, dimension, dimension), glm::vec3(dimension, -dimension, dimension),
		// Top face
		glm::vec3(-dimension, dimension, -dimension), glm::vec3(dimension, dimension, -dimension), glm::vec3(-dimension, dimension, dimension), glm::vec3(dimension, dimension, dimension),
		// Bottom face
		glm::vec3(dimension, -dimension, -dimension), glm::vec3(-dimension, -dimension, -dimension), glm::vec3(dimension, -dimension, dimension), glm::vec3(-dimension, -dimension, dimension),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] = 
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	/*
	//FOR(i, 24)
	for(int i=0; i<24; i++)
	{
		vboRenderData.addData(&vSkyBoxVertices[i], sizeof(glm::vec3));		
		vboRenderData.addData(&vSkyBoxNormals[i/4], sizeof(glm::vec3));
		vboRenderData.addData(&vSkyBoxTexCoords[i%4], sizeof(glm::vec2));
	}

	vboRenderData.uploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), 0);	
	// Normal vectors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)(sizeof(glm::vec3)));
	// Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)(2*sizeof(glm::vec3)));
	*/
	for(int i=0; i<24; i++)
	{
		vboRenderData.addData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		vboRenderData.addData(&vSkyBoxTexCoords[i%4], sizeof(glm::vec2));
		vboRenderData.addData(&vSkyBoxNormals[i/4], sizeof(glm::vec3));
	}

	vboRenderData.uploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
}

/*-----------------------------------------------

Name:	renderSkybox

Params:	none

Result: Guess what it does :)

/*---------------------------------------------*/

void CSkybox::renderSkybox()
{
	glDepthMask(0);
	glBindVertexArray(uiVAO);
	//FOR(i, 6)
	for(int i = 0; i<6; i++)
	{
		tTextures[i].bindTexture(40+i);
		glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
	}
	glDepthMask(1);
	//glActiveTexture(GL_TEXTURE0);
}

/*-----------------------------------------------

Name:	loadSkybox

Params:	paths of skybox pictures

Result:	Loads skybox and creates VAO and VBO for it.

/*---------------------------------------------*/

void CSkybox::releaseSkybox()
{
	//FOR(i, 6)
	for(int i=0; i<6; i++) tTextures[i].releaseTexture();
	glDeleteVertexArrays(1, &uiVAO);
	vboRenderData.releaseVBO();
}