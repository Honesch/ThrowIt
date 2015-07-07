/*
#include "objLoader.h"

using namespace Assimp;

objLoader::objLoader(void)
{
}


objLoader::~objLoader(void)
{
}


Modell* objLoader::loadModell( const char * modelpath)
{
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
		return nullptr;
	}

	scene = importer.ReadFile( modelpath,aiProcessPreset_TargetRealtime_Quality);

	if( !scene)
	{
		printf("%s\n", importer.GetErrorString());
		return nullptr;
	}else{
		aiMesh* mesh = scene->mMeshes[0];

		int indexcount = mesh->mNumFaces * 3;
		int vertexcount = mesh->mNumVertices * 3;

		float *vertices = 0;
		unsigned int *indices = 0;

		
		 //Vielleicht ist es besser mit std::vector<...> zu arbeiten, statt mit  "new array[]"
		 // Für jedes new -> muss ein entsprechendes delete da sein! -> Faustregel!!
		 
		vertices = new float[mesh->mNumVertices * 3];	// HIER HAT EIN * 3 gefehlt!!
		indices = new unsigned int[mesh->mNumFaces * 3]; //sind nur dreiecke da beim laden Triangulate verwendet wird 

		for (unsigned int i = 0; i < mesh->mNumVertices * 3; ++i) {
 
			vertices[i] = 0.0f;
        }

		unsigned int index = 0;


		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace* face = &mesh->mFaces[i];
 
            memcpy(&indices[index], face->mIndices, 3 * sizeof(unsigned int));
            index += 3;
        }


		index = 0;
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			
				//ACHTUNG!
				//vertices hat eine Größe von mNumVertices.
			//	Ihr habt hier ursprünglich jedoch 3 x soviel reingespeichert!!
			
			memcpy(&vertices[index], mesh->mVertices, 3 * sizeof(float));
            index += 3;
        }
		Modell* outputModell;

		
		// Ihr übergebt hier vertexCount und indexCount
		// Im Konstruktor von Modell wird vertexCount jedoch mit 3 multipliziert (glBufferData) und 
		// indexCount nicht! Das ist irgendwie nicht intuitiv.
		// DAS HEIßT AUCH: Dass in der main.cpp das Standard Triangle Modell eine falsche Anzahl von vertex Koordinaten an den
		// Modelkonstruktor übergibt (nämlich 15, statt 5)
		
		outputModell = new Modell(glm::mat4(1.0f), vertices, vertexcount, indices, indexcount);

		return outputModell;
	}
}
*/