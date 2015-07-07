#include "Modell.h"
#include "Texture.h"

using namespace Assimp;

Modell::Modell(glm::mat4& matrix, const char * modelpath)
{
	mat = matrix;

	Importer importer;
	const aiScene* scene = NULL;
	
	//gibt es die Datei
	std::ifstream fin(modelpath);
	if(!fin.fail()) {
		fin.close();
	}
	else{
		printf("Fehler beim laden des Modells: %s\n", modelpath);
		printf("%s\n", importer.GetErrorString());
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	scene = importer.ReadFile( modelpath,/* aiProcess_Triangulate | */aiProcessPreset_TargetRealtime_Quality);

	if( !scene)
	{
		printf("%s\n", importer.GetErrorString());
		system("PAUSE");
		exit(EXIT_FAILURE);
	}else{
		mesh = scene->mMeshes[0];

		int indexcount = mesh->mNumFaces * 3;
		int vertexcount = mesh->mNumVertices *3;
		int normcount = mesh->mNumVertices *3;
		int uvCoordcount = mesh->mNumVertices *2;

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) 
		{
            const aiFace* face = &mesh->mFaces[i];
			
			unsigned int index = face->mIndices[0];
			indices.push_back(index);

			index = face->mIndices[1];
			indices.push_back(index);

			index = face->mIndices[2];
			indices.push_back(index);
        }


		//index = 0;
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) 
		{
			glm::vec3 positions;

			positions.x = mesh->mVertices[i].x;
			positions.y = mesh->mVertices[i].y;
			positions.z = mesh->mVertices[i].z;

			pos.push_back(positions);

			glm::vec3 norm;
			norm.x = mesh->mNormals[i].x;
			norm.y = mesh->mNormals[i].y;
			norm.z = mesh->mNormals[i].z;

			normals.push_back(norm);

			glm::vec2 tex;
			//es koennten auch mehrere texturen am objekt sein. Wir supporten vorerst nur eine
			if (mesh->mTextureCoords[0]) 
			{
				tex.x = mesh->mTextureCoords[0][i].x;
				tex.y = mesh->mTextureCoords[0][i].y;

				uvs.push_back(tex);
			}
		}
		if (mesh->mMaterialIndex >= 0) {

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D colorDIF (0.f,0.f,0.f);
		aiColor3D colorSPEC (0.f,0.f,0.f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE,colorDIF);
		material->Get(AI_MATKEY_COLOR_DIFFUSE,colorSPEC);

		diffuseMAT.r = colorDIF.r;
		diffuseMAT.g = colorDIF.g;
		diffuseMAT.b = colorDIF.b;

		specularMAT.r = colorSPEC.r;
		specularMAT.g = colorSPEC.g;
		specularMAT.b = colorSPEC.b;

		Texture textureloader;
		diffuseMaps = textureloader.loadMaterialTextures(modelpath, material, aiTextureType_DIFFUSE, "diffuse");
		//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		specularMaps = textureloader.loadMaterialTextures(modelpath, material, aiTextureType_SPECULAR, "specular");
		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	calcHitbox();
	
	positioncount = vertexcount;
	indicescount = indexcount;
	normalscount = normcount;
	uvcount = uvCoordcount;

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positioncount/*Anzahl Knoten*/  * sizeof(float), &pos[0], GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicescount /*Anzahl Indizes*/ * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normalscount /*Anzahl Indizes*/ * sizeof(float), &normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvcount /*Anzahl Indizes*/ * sizeof(float), &uvs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	minX = getMinX();
	minY = getMinY();
	minZ = getMinZ();
	maxX = getMaxX();
	maxY = getMaxY();
	maxZ = getMaxZ();
	box = calcBox();
}

float Modell::getYposAt(float x, float z){
	
	//Ein Versun an Interpolation. Ist unvollstaendig.
	/*
	float min1SumOfAbsolutes = 100;
	float min2SumOfAbsolutes = 100;
	float min3SumOfAbsolutes = 100;
	float min4SumOfAbsolutes = 100;
	
	float currentX, currentZ, currentY;
	
	float y1 = 0;
	float y2 = 0;
	float y3 = 0;
	float y4 = 0;

	float x1,x2,x3,x4;
	float z1,z2,z3,z4;
	

	for(unsigned int i=0; i<pos.size();i++){
		float currentSum = abs(pos[i].x-x) + abs(pos[i].z-z);
		currentX = pos[i].x;
		currentZ = pos[i].z;
		currentY = pos[i].y;

		if(currentSum < min1SumOfAbsolutes){
			min1SumOfAbsolutes = currentSum;
			x1 = currentX;
			z1 = currentZ;
			y1 = currentY;
		} else if(currentSum < min2SumOfAbsolutes){
			min2SumOfAbsolutes = currentSum;
			x2 = currentX;
			z2 = currentZ;
			y2 = currentY;
		} else if(currentSum < min3SumOfAbsolutes){
			min3SumOfAbsolutes = currentSum;
			x3 = currentX;
			z3 = currentZ;
			y3 = currentY;
		} else if(currentSum < min4SumOfAbsolutes){
			min4SumOfAbsolutes = currentSum;
			x4 = currentX;
			z4 = currentZ;
			y4 = currentY;
		}
	}

	float out = ((x/(x1+x2))*y1+(x/(x1+x2))*y2);

	printf("%.2f\n",out);
	return (out);
	*/
	
	//Nimmt einfach y des naehesten vertex
	float minabs = 100;
	float y;

	for(unsigned int i=0; i<pos.size();i++){
		float currentSum = abs(pos[i].x-x) + abs(pos[i].z-z);
		if(currentSum<minabs){
			minabs = currentSum;
			y = pos[i].y;
		}
	}
	return y;
	
}

void Modell::calcHitbox()
{
	GLfloat min_x, max_x, min_y, max_y, min_z, max_z;

	min_x = max_x = pos[0].x;
	min_y = max_y = pos[0].y;
	min_z = max_z = pos[0].z;
	for ( int i = 0; i != pos.size(); i++) 
	{
		if (pos[i].x < min_x) min_x = pos[i].x;
		if (pos[i].x > max_x) max_x = pos[i].x;
		if (pos[i].y < min_y) min_y = pos[i].y;
		if (pos[i].y > max_y) max_y = pos[i].y;
		if (pos[i].z < min_z) min_z = pos[i].z;
		if (pos[i].z > max_z) max_z = pos[i].z;
  }
  //glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
    glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
	hitboxRadius = max_x - min_x;


}

Modell::~Modell(void)
{
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);

	glDeleteVertexArrays(1, &vao);
}

void Modell::draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter)
{
	glUseProgram(handle);
	
	if(diffuseMaps.size() > 0)
	{
		glActiveTexture(GL_TEXTURE0 + textureI);
		glBindTexture(GL_TEXTURE_2D, diffuseMaps[0]);

		if (minificationFilter == GL_NONE)
		{
			minificationFilter = magnificationFilter;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter);

		auto tex_location = glGetUniformLocation(handle, "color_texture");
		glUniform1i(tex_location, textureI);

		auto dif_location = glGetUniformLocation(handle, "diffuse");
		glUniform3f(dif_location, diffuseMAT.x, diffuseMAT.y, diffuseMAT.z);

		auto spec_location = glGetUniformLocation(handle, "specular");
		glUniform3f(spec_location, specularMAT.x, specularMAT.y, specularMAT.z);
	}
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicescount , GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//__________________________________________________________________________________________________

void Modell::draw(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullingEnabled){
	

	//Culling
	if(cullingEnabled){
		//if(!frustum->InFrustum(center,hitboxRadius)) return;
		if(!frustum->boxInFrustum(calcBox())) return;		
	}
	//if(minX == -50) return; //Skybo

	glUseProgram(handle);
	
	if(diffuseMaps.size() > 0)
	{
		glActiveTexture(GL_TEXTURE0 + textureI);
		glBindTexture(GL_TEXTURE_2D, diffuseMaps[0]);

		if (minificationFilter == GL_NONE)
		{
			minificationFilter = magnificationFilter;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter);

		auto tex_location = glGetUniformLocation(handle, "color_texture");
		glUniform1i(tex_location, textureI);

		auto dif_location = glGetUniformLocation(handle, "diffuse");
		glUniform3f(dif_location, diffuseMAT.x, diffuseMAT.y, diffuseMAT.z);

		auto spec_location = glGetUniformLocation(handle, "specular");
		glUniform3f(spec_location, specularMAT.x, specularMAT.y, specularMAT.z);
	}
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicescount , GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//printf("%i vertices culled\n",vertices_culled);
}

float Modell::getMaxX(){
	float max=0;
	for(int i=0; i<pos.size() ; i++){
		if(pos[i].x > max){
			max = pos[i].x;
		}
	}
	return max;
}
float Modell::getMaxY(){
	float max=0;
	for(int i=0; i<pos.size() ; i++){
		if(pos[i].y > max){
			max = pos[i].y;
		}
	}
	return max;
}
float Modell::getMaxZ(){
	float max=0;
	for(int i=0; i<pos.size() ; i++){
		if(pos[i].z > max){
			max = pos[i].z;
		}
	}
	return max;
}


float Modell::getMinX(){
	float max=10000;
	for(int i=0; i<pos.size() ; i++){
		if(pos[i].x < max){
			max = pos[i].x;
		}
	}
	return max;
}
float Modell::getMinY(){
	float min=10000;
	for(int i=0; i<pos.size() ; i++){
		if(pos[i].y < min){
			min = pos[i].y;
		}
	}
	return min;
}
float Modell::getMinZ(){
	float min=10000;
	for(int i=0; i<pos.size() ; i++){
		if(pos[i].z < min){
			min = pos[i].z;
		}
	}
	return min;
}

AABox Modell::calcBox(){
	glm::vec3 corner(minX,minY,minZ);
	glm::vec3 verschiebung(mat[3].x, mat[3].y, mat[3].z);
	glm::vec3 globalcorner = corner + verschiebung;

	return  AABox(globalcorner,2*maxX,2*maxY,2*maxZ);
}

void Modell::updateHitbox(){
	glm::vec3 corner(minX,minY,minZ);
	glm::vec3 verschiebung(mat[3].x, mat[3].y, mat[3].z);
	glm::vec3 globalcorner = corner + verschiebung;
	box.setBox(globalcorner,2*maxX,2*maxY,2*maxZ);
}
