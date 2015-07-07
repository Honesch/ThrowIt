#pragma once

#include <string>
#include <glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <assimp\scene.h>
#include <iostream>

class Texture
{
public:
	Texture(/*const std::string& path*/);
	~Texture(void);

	void bind(int unit);
	std::vector<GLuint> loadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType type, std::string typeName);
	GLuint loadTexture(std::string filenameString, GLenum minificationFilter, GLenum magnificationFilter);
	
private:
	GLuint handle;
	std::vector<GLuint> textures_loaded;
};

