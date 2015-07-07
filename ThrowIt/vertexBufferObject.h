#pragma once
#include <vector>
#include <glew.h>
#include <glm\glm.hpp>
#include "windows.h"



/********************************

Class:	CVertexBufferObject

Purpose:	Wraps OpenGL vertex buffer
			object.

********************************/

class CVertexBufferObject
{
public:
	void createVBO(int a_iSize = 0);
	void releaseVBO();

	void* mapBufferToMemory(int iUsageHint);
	void* mapSubBufferToMemory(int iUsageHint,unsigned int uiOffset,unsigned int uiLength);
	void unmapBuffer();

	void bindVBO(int a_iBufferType = GL_ARRAY_BUFFER);
	void uploadDataToGPU(int iUsageHint);
	
	void addData(void* ptrData,unsigned int uiDataSize);

	void* getDataPointer();
	unsigned int getBuffer();

	CVertexBufferObject();

private:
	unsigned int uiBuffer;
	int iSize;
	int iBufferType;
	std::vector<BYTE> data;

	bool bDataUploaded;
};