#include "Shaderloader.h"

using namespace std;

Shaderloader::Shaderloader(void)
{
	printf("Skyboxshader..");
}


Shaderloader::~Shaderloader(void)
{
}

GLuint Shaderloader::LoadShaders(const char * vertex_path, const char * fragment_path)
{
	GLuint Vertexhandle = glCreateShader(GL_VERTEX_SHADER);
    GLuint Fragmenthandle = glCreateShader(GL_FRAGMENT_SHADER);

	// -.-.-.-.- shader Programme laden -.-.-.-.-.-.-
	string VertexShaderCode;
    ifstream VertexShaderStream(vertex_path, ios::in);
    if(VertexShaderStream.is_open())
    {
        string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_path, ios::in);
    if(FragmentShaderStream.is_open()){
        string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- vertex Shader Compilieren -.-.-.-.-.-.- 
	char const * VertexPtr = VertexShaderCode.c_str();
    glShaderSource(Vertexhandle, 1, &VertexPtr , NULL);
    glCompileShader(Vertexhandle);

	// test ob erfolgreich
	GLint succeded;
	glGetShaderiv(Vertexhandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Vertexhandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Vertexhandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- fragment Shader Compilieren -.-.-.-.-.-.- 
	char const * FragmentPtr = FragmentShaderCode.c_str();
    glShaderSource(Fragmenthandle, 1, &FragmentPtr , NULL);
    glCompileShader(Fragmenthandle);

	// test ob erfolgreich
	glGetShaderiv(Fragmenthandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Fragmenthandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Fragmenthandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- Shader Linken -.-.-.-.-.-.- 
	GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, Vertexhandle);
    glAttachShader(programHandle, Fragmenthandle);

	glBindFragDataLocation(programHandle, 0, "fragColor"); //Output "fragColor" auf position 0 legen
	glLinkProgram(programHandle);
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- testen ob programm richtig angelegt -.-.-.-.-.-.- 
	glGetProgramiv(programHandle, GL_LINK_STATUS, &succeded);
	if (!succeded)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(programHandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(programHandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	glDeleteShader(Vertexhandle);
    glDeleteShader(Fragmenthandle);

	return programHandle;
}

GLuint Shaderloader::LoadTransformFeedbackShaders(const char * vertex_path,  const char * geo_path, const GLchar* feedbackVaryings[], int varyingcount)
{
	// set up the handle to the individual shaders
	GLuint Vertexhandle = glCreateShader(GL_VERTEX_SHADER);
	GLuint Geometryhandle = glCreateShader(GL_GEOMETRY_SHADER);

	// -.-.-.-.- shader Programme laden -.-.-.-.-.-.-
	//load vertex shader
	string VertexShaderCode;
    ifstream VertexShaderStream(vertex_path, ios::in);
    if(VertexShaderStream.is_open())
    {
        string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
	

	//load geometry shader
	string GeometryShaderCode;
    ifstream GeometryShaderStream(geo_path, ios::in);
    if(GeometryShaderStream.is_open())
	{
        string Line = "";
        while(getline(GeometryShaderStream, Line))
            GeometryShaderCode += "\n" + Line;
        GeometryShaderStream.close();
    }
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- vertex Shader Compilieren -.-.-.-.-.-.- 
	//compile vertex shader
	char const * VertexPtr = VertexShaderCode.c_str();
    glShaderSource(Vertexhandle, 1, &VertexPtr , NULL);
    glCompileShader(Vertexhandle);

	// test ob erfolgreich
	//test vertex shader
	GLint succeded;
	glGetShaderiv(Vertexhandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Vertexhandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Vertexhandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-


	// -.-.-.-.- geometry Shader Compilieren -.-.-.-.-.-.- 
	//compile geometry shader
	char const * GeometryPtr = GeometryShaderCode.c_str();
    glShaderSource(Geometryhandle, 1, &GeometryPtr , NULL);
    glCompileShader(Geometryhandle);

	// test ob erfolgreich
	//test geometry shader
	glGetShaderiv(Geometryhandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Geometryhandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Geometryhandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- Shader Linken -.-.-.-.-.-.- 
	//link the shaders to a program
	GLuint programHandle = glCreateProgram();

	//Transform feedback outvalue
	//const GLchar* feedbackVaryings[] = { "outValue" };
	glTransformFeedbackVaryings(programHandle, varyingcount, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(programHandle, Vertexhandle);
	glAttachShader(programHandle, Geometryhandle); 

	//glBindFragDataLocation(programHandle, 0, "fragColor"); //Output "fragColor" auf position 0 legen
	glLinkProgram(programHandle);
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- testen ob programm richtig angelegt -.-.-.-.-.-.- 
	//test program
	glGetProgramiv(programHandle, GL_LINK_STATUS, &succeded);
	if (!succeded)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(programHandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(programHandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	glDeleteShader(Vertexhandle);
	glDeleteShader(Geometryhandle);

	return programHandle;
}

GLuint Shaderloader::LoadShaders(const char * vertex_path, const char * fragment_path, const char * geo_path)
{
	// set up the handle to the individual shaders
	GLuint Vertexhandle = glCreateShader(GL_VERTEX_SHADER);
    GLuint Fragmenthandle = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint Geometryhandle = glCreateShader(GL_GEOMETRY_SHADER);

	// -.-.-.-.- shader Programme laden -.-.-.-.-.-.-
	//load vertex shader
	string VertexShaderCode;
    ifstream VertexShaderStream(vertex_path, ios::in);
    if(VertexShaderStream.is_open())
    {
        string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
	
	//load fragment shader
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_path, ios::in);
    if(FragmentShaderStream.is_open())
	{
        string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

	//load geometry shader
	string GeometryShaderCode;
    ifstream GeometryShaderStream(geo_path, ios::in);
    if(GeometryShaderStream.is_open())
	{
        string Line = "";
        while(getline(GeometryShaderStream, Line))
            GeometryShaderCode += "\n" + Line;
        GeometryShaderStream.close();
    }
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- vertex Shader Compilieren -.-.-.-.-.-.- 
	//compile vertex shader
	char const * VertexPtr = VertexShaderCode.c_str();
    glShaderSource(Vertexhandle, 1, &VertexPtr , NULL);
    glCompileShader(Vertexhandle);

	// test ob erfolgreich
	//test vertex shader
	GLint succeded;
	glGetShaderiv(Vertexhandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Vertexhandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Vertexhandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- fragment Shader Compilieren -.-.-.-.-.-.- 
	//compile fragment shader
	char const * FragmentPtr = FragmentShaderCode.c_str();
    glShaderSource(Fragmenthandle, 1, &FragmentPtr , NULL);
    glCompileShader(Fragmenthandle);

	// test ob erfolgreich
	//test fragment shader
	glGetShaderiv(Fragmenthandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Fragmenthandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Fragmenthandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- geometry Shader Compilieren -.-.-.-.-.-.- 
	//compile geometry shader
	char const * GeometryPtr = GeometryShaderCode.c_str();
    glShaderSource(Geometryhandle, 1, &GeometryPtr , NULL);
    glCompileShader(Geometryhandle);

	// test ob erfolgreich
	//test geometry shader
	glGetShaderiv(Geometryhandle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(Geometryhandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(Geometryhandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- Shader Linken -.-.-.-.-.-.- 
	//link the shaders to a program
	GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, Vertexhandle);
    glAttachShader(programHandle, Fragmenthandle);
	glAttachShader(programHandle, Geometryhandle);

	glBindFragDataLocation(programHandle, 0, "fragColor"); //Output "fragColor" auf position 0 legen
	glLinkProgram(programHandle);
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	// -.-.-.-.- testen ob programm richtig angelegt -.-.-.-.-.-.- 
	//test program
	glGetProgramiv(programHandle, GL_LINK_STATUS, &succeded);
	if (!succeded)
	{
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(programHandle, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(programHandle, logSize, NULL, message);
		delete[] message;
	}
	// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	glDeleteShader(Vertexhandle);
    glDeleteShader(Fragmenthandle);
	glDeleteShader(Geometryhandle);

	return programHandle;
}
