#pragma once

#include <glew.h>
#include <gl\GL.h>
#include <string>
#include <fstream>


class Shaderloader
{
public:
	Shaderloader(void);
	~Shaderloader(void);
	GLuint LoadShaders(const char * vertex_path, const char * fragment_path);
	GLuint LoadShaders(const char * vertex_path, const char * fragment_path, const char * geo_path);
	GLuint LoadTransformFeedbackShaders(const char * vertex_path, const char * geo_path,const GLchar* feedbackVaryings[], int varyingcount);
};

