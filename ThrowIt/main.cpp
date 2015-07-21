#include <iostream>
#include "windows.h"
#include <glew.h>
#include <gl\GL.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#include "Shaderloader.h"
#include "Modell.h"
#include "Kamera.h"
#include "DirectionalLight.h"
#include "Player.h"
#include "ThrowAble.h"
#include "Texture.h"
#include "text2D.h"
#include "texture2D.h"
#include "Waterplane.h"
#include "Partikelsystem.h"
#include "FrustumG.h"
#include "skybox.h"


//CONSTANTS
#define VIEW_FRUSTUM_CULLING_ENABLED true
#define START_X 0.0f
//#define START_Y -1.0f 
#define START_Y 0.0f 
#define START_Z 0.0f
#define WURFANZAHL_MAX 2
#define WIDTH 800
#define HEIGHT 600
#define RUNDENZEIT 120.0f

//FUNCTIONS
void initWorld(GLuint proghandle, GLFWwindow* window);
void initWindow(int argc, char** argv);
glm::mat4 initShadowMapsAndLight();
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleMouseMove(GLFWwindow* w);
void update(float time_delta);
void draw(GLuint progHandle, glm::mat4 LPV, Player* plaxerX, bool inverse); //inverse wird fuer die reflexion im wasser benutzt. Spiegelt an der y achse
void drawBlendet(GLuint progHandle, glm::mat4 LPV);
void cleanup();
void controlBoundery(int player);
void shoot(int player, Player* _player);
void renderShadowmaps(glm::mat4 PV);
void deleteShadowmaps();
void handle_mouse_button();

void initWaterRecources();
void renderWaterReflexion(GLuint progHandle, glm::mat4 LPV, Player* playerX, int playernumber);
void renderWaterRefraction(GLuint progHandle, glm::mat4 LPV, Player* playerX, int playernumber);
void renderWaterResources(GLuint progHandle, glm::mat4 LPV, Player* playerX, int playernumber);
void drawWater(Player* playerX, double delta_t, int textureIndex , int playernumber);
void deleteWater();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
bool collide(Modell* a, Modell* b);

void initParticle();
void drawParticle(float time_delta, Player* playerx);

void initSkybox();

static void APIENTRY DebugCallbackAMD(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
//glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

//WINDOW
GLFWwindow* window;

//SHADER AND PROGRAMS
Shaderloader loader;
GLuint programHandle;
GLuint zShaders;
GLuint waterShaders;
GLuint skyboxShaders;

//BUFFER
GLuint fb; //Framebuffer fuer Shadowmap
GLuint fb_reflexion[2]; //Framebuffer fuer die Reflexion am Wasser
GLuint fb_refraction[2]; //Framebuffer fuer die Wasserrefraction

//TEXTURES
GLuint ztex;
GLuint reflexiontex[2]; //Reflexionstextur fuers Wasser
GLuint refractiontex[2]; //Refractionstextur fuers Wasser
GLuint WaterNormalMap;
GLuint WaterDuDvMap;

//OBJECTS
Modell* boden;
Modell* boden2;
Modell* sphere;
Modell* cube;
Modell* zylinder;
Modell* cone;

ThrowAble** thrownObjekts; //objekte die gerade geworfen wurden

ThrowAble* bulletA1;
ThrowAble* bulletB1;
ThrowAble* bulletC1;
ThrowAble* bulletA2;
ThrowAble* bulletB2;
ThrowAble* bulletC2;

Player* player1;
Player* player2;

Waterplane* water;
Partikelsystem* particle;
FrustumG* frustum1;
FrustumG* frustum2;

//CAMERA AND LIGHT
Kamera* cam;
DirectionalLight* light;
CSkybox skybox;


int maplaenge = 28;
int mapbreite = 28;
float time_delta;

//SPIELEEINSTELLUNGEN
bool gameOver = false;
bool viewFrustumCulling;
bool blendet;
bool frameTime;
bool wireFrame;
bool particlecount;
GLuint TextureSamplingMode;
GLuint MipMappingMode;


int main(int argc, char** argv)
{
	//-------------------------- INITIALISATIONS --------------------------
	
	initWindow(argc, argv);

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	programHandle= loader.LoadShaders("../Shader/vertexshader.vert", "../Shader/fragmentshader.frag");

	initWorld(programHandle, window);

	glm::mat4 PV = initShadowMapsAndLight();

	initText2D("../Models/letters.DDS");

	initWaterRecources();

	initParticle();

	initSkybox();

	double time = glfwGetTime();
	double startingtime = glfwGetTime();
	
	//-------------------------- MAIN LOOP -----------------------------

	while (!glfwWindowShouldClose(window))
	{
		
		glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_TEXTURE_2D);
		
		
		renderShadowmaps(PV);
			
		glm::mat4 L(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);

		glm::mat4 LPV = L * PV;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ztex);
		glUseProgram(programHandle);

		

		auto tex_location = glGetUniformLocation(programHandle, "shadowMap");
		glUniform1i(tex_location, 0);

		//fuer die Metreialbeleuchtung
		auto lightpos_location = glGetUniformLocation(programHandle, "lightPOS");
		glUniform3f(lightpos_location, light->pos.x, light->pos.y, light->pos.z);

		

		double time_new = glfwGetTime();
		time_delta = (float)(time_new - time);
		time = time_new;
		if(frameTime)
		{
			std::cout << "frametime: " <<time_delta * 1000 << "ms"
				<< "     _    "<<1.0/time_delta<< " fps"<<std::endl;
		}
		glfwPollEvents();
		

		//Das Wasser rendern muss seperat hier gemacht werden, da sonst das Colorbufferbit zwischen den Playern geloescht wird
		if(player1 != nullptr || player2 != nullptr)
		{
			renderWaterResources(programHandle, LPV, player1, 0);
			renderWaterResources(programHandle, LPV, player2, 1);

		}

		if(player1 != nullptr)
		{
			glViewport(0,0,WIDTH/2,HEIGHT);
			
			//controlBoundery(1);
			player1->playerkamera->move(time_delta,boden);
			player1->playerkamera->updateY(boden,1);
			
			player1->updateBodyToCamRelation();
			player1->updateOrbiterToCamRelation(time_delta);
			player1->updateShootingForce(time_delta);
			
			draw(programHandle, LPV, player1, false);
			
			//player1->updateOrbiterToCamRelation(time_delta,frustum1, player1,sphere);
			glm::vec3 localdir = player1->playerkamera->calculateLocalViewdirection();
			frustum1->setCamDef(player1->playerkamera->getPosition()-localdir,
								localdir,
								glm::vec3(0,1,0)); 
						
			drawWater(player1,time_delta,7,0);

			drawParticle(time_delta, player1);

			char text[10];			
			sprintf(text,"%.0f HP",(float)player1->hitpoints);
			printText2D(text, 10, 10, 80); 

			if(player1->currentBullet=='A'){
				sprintf(text,">A: %i",player1->ammo1);
				printText2D(text, 530, 100, 40);
			}else{
				sprintf(text,"A: %i",player1->ammo1);
				printText2D(text, 600, 100, 40);
			}

			if(player1->currentBullet=='B'){
				sprintf(text,">B: %i",player1->ammo2);
				printText2D(text, 530, 60, 40);
			}else{
				sprintf(text,"B: %i",player1->ammo2);
				printText2D(text, 600, 60, 40);
			}

			if(player1->currentBullet=='C'){
				sprintf(text,">C: %i",player1->ammo3);
				printText2D(text, 530, 20, 40);
			}else{
				sprintf(text,"C: %i",player1->ammo3);
				printText2D(text, 600, 20, 40);
			}
			
			switch(player1->shootingForceStage){
			case 0: sprintf(text,"|     |");
				break;
			case 1: sprintf(text,"|#    |");
				break;
			case 2: sprintf(text,"|##   |");
				break;
			case 3: sprintf(text,"|###  |");
				break;
			case 4: sprintf(text,"|#### |");
				break;
			case 5: sprintf(text,"|#####|");
				break;
			}
			printText2D(text, 20, 100, 40);

			sprintf(text,"%.0f sek",RUNDENZEIT-time+startingtime);
			printText2D(text, 10, 500, 80);


			drawBlendet(programHandle, LPV);
			

			if(RUNDENZEIT-time+startingtime < 0){//war 100
				if(!gameOver && player1->hitpoints > player2->hitpoints){
					player1->playerkamera->resetkeys();
					player2->playerkamera->resetkeys();
					delete(player1);
					player1 = nullptr;
					delete(player2);
					player2 = nullptr;					
					std::cout << "\n\nSPIELER 1 HAT GEWONNEN!" << std::endl;
					gameOver = true;
				}else if(!gameOver && player2->hitpoints > player1->hitpoints){
					player1->playerkamera->resetkeys();
					player2->playerkamera->resetkeys();
					delete(player1);
					player1 = nullptr;
					delete(player2);
					player2 = nullptr;
					std::cout << "\n\nSPIELER 2 HAT GEWONNEN!" << std::endl;
					gameOver = true;
				}else if(!gameOver && player1->hitpoints == player2->hitpoints){
					player1->playerkamera->resetkeys();
					player2->playerkamera->resetkeys();
					delete(player1);
					player1 = nullptr;
					delete(player2);
					player2 = nullptr;
					std::cout << "\n\nUNENTSCHIEDEN!" << std::endl;

					/*char text[20];			
					sprintf(text,"UNENTSCHIEDEN");
					printText2D(text, 10, 10, 80);*/ 

					gameOver = true;
				}

			}
		}
		if(player2 != nullptr)
		{
			glViewport(WIDTH/2,0, WIDTH/2, HEIGHT);
			//controlBoundery(2);
			player2->playerkamera->move(time_delta,boden);
			player2->playerkamera->updateY(boden,2);
			player2->updateBodyToCamRelation();	
			player2->updateOrbiterToCamRelation(time_delta);

			draw(programHandle, LPV, player2, false);

			drawWater(player2,time_delta,7,1);		

			drawParticle(time_delta, player2);
		
			//player2->updateOrbiterToCamRelation(time_delta,frustum2);
			//frustum2->setCamDef(player2->playerkamera->getPosition(),glm::vec3(0,0,-1),glm::vec3(0,-1,0));

			char text[10];	
			if(player2->currentBullet=='A'){
				sprintf(text,">A: %i",player2->ammo1);
				printText2D(text, 530, 100, 40);
			}else{
				sprintf(text,"A: %i",player2->ammo1);
				printText2D(text, 600, 100, 40);
			}

			if(player2->currentBullet=='B'){
				sprintf(text,">B: %i",player2->ammo2);
				printText2D(text, 530, 60, 40);
			}else{
				sprintf(text,"B: %i",player2->ammo2);
				printText2D(text, 600, 60, 40);
			}

			if(player2->currentBullet=='C'){
				sprintf(text,">C: %i",player2->ammo3);
				printText2D(text, 530, 20, 40);
			}else{
				sprintf(text,"C: %i",player2->ammo3);
				printText2D(text, 600, 20, 40);
			}

			switch(player2->shootingForceStage){
			case 0: sprintf(text,"|     |");
				break;
			case 1: sprintf(text,"|#    |");
				break;
			case 2: sprintf(text,"|##   |");
				break;
			case 3: sprintf(text,"|###  |");
				break;
			case 4: sprintf(text,"|#### |");
				break;
			case 5: sprintf(text,"|#####|");
				break;
			}
			printText2D(text, 20, 100, 40);
			
			sprintf(text,"%.0f HP",(float)player2->hitpoints);
			printText2D(text, 10, 10, 80); 

			drawBlendet(programHandle, LPV);
		}				

		
		if(player2 != nullptr)
			player2->playerkamera->moveNumpad(time_delta);
		if(player1 != nullptr)
			handleMouseMove(window);
		update(time_delta);
		glfwSwapBuffers(window);
				
		glUseProgram(0);
	}

	glDeleteProgram(programHandle);
	cleanup();
}

void initWorld(GLuint proghandle, GLFWwindow* window)
{
	glUseProgram(proghandle);


	//PLAYER1
	glm::vec3 posP1 = glm::vec3(0,10,-25);

	Kamera* player1cam = new Kamera(WIDTH, HEIGHT, window, posP1);
	player1cam->rotation = glm::vec3(0,0,0);

	glm::mat4 p1matrix = glm::translate(posP1);
	Modell* p1mod = new Modell(p1matrix,  "../Models/player1Sphere.obj");

	glm::mat4 o1matrix = glm::translate(posP1);
	glm::mat4 orbiteroffset = glm::translate(glm::vec3(0,0,-5));
	Modell* orbiter1 = new Modell(o1matrix * orbiteroffset, "../Models/orbiter.obj");

	player1 = new Player(p1mod,orbiter1,player1cam,1,'A',3,15);
	player1cam->setPlayerSpeed(6.0);

	//PLAYER2
	glm::vec3 posP2 = glm::vec3(0,10,25);

	Kamera* player2cam = new Kamera(WIDTH, HEIGHT, window, posP2);
	player2cam->rotation = glm::vec3(0,180,0);

	glm::mat4 p2matrix = glm::translate(posP2);
	Modell* p2mod = new Modell(p2matrix , "../Models/player1Sphere.obj");	

	glm::mat4 o2matrix = glm::translate(posP2);
	Modell* orbiter2 = new Modell(o2matrix * orbiteroffset, "../Models/orbiter.obj");
	
	player2 = new Player(p2mod,orbiter2,player2cam,2,'A',3,15);
	player2cam->setPlayerSpeed(6.0);

	//VIEW FRUSTUMS
	frustum1 = new FrustumG();
	frustum2 = new FrustumG();

	frustum1->setCamInternals(45.0f,(800.0f/600.0f),1.0f,60.0f);
	frustum2->setCamInternals(45.0f,(800.0f/600.0f),2.0f,10.0f);

	//CUBE
	glm::mat4 cuberotation = glm::rotate(glm::radians(45.0f),glm::vec3(0,-1,0));
	glm::mat4 matrix = glm::translate(glm::vec3(0.0f, 3.0f ,0.0f));
	cube = new Modell(matrix*cuberotation, "../Models/testcube.obj");

	//BODEN
	glm::mat4 bodenmatrix = glm::translate(glm::vec3(0.0f, -1.0f ,0.0f));
	boden = new Modell(bodenmatrix, "../Models/Boden.obj");

	//BODEN2
	glm::mat4 boden2matrix = glm::translate(glm::vec3(10.0f, 1.0f ,10.0f));
	boden2 = new Modell(boden2matrix, "../Models/Boden2.obj");
	
	//SPHERE
	glm::mat4 spherematrix = glm::translate(glm::vec3(0.0f, 10.0f ,0.0f));
	sphere = new Modell(spherematrix, "../Models/sphere.obj");

	//DEBUG OBJECTS
	/*
	glm::mat4 orbmatrix = glm::translate(glm::vec3(0.0f, 2.0f ,0.0f));
	o_nbl = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_nbr = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_nbl = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_nbr = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_ftl = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_ftr = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_ftl = new Modell(orbmatrix, "../Models/orbiter.obj");
	o_ftr = new Modell(orbmatrix, "../Models/orbiter.obj");
	*/

	

	glUseProgram(0);
	glUseProgram(-2);

	//Initialisierung der Wurfarrays
	thrownObjekts = new ThrowAble*[WURFANZAHL_MAX];
	thrownObjekts[0] = nullptr;
	thrownObjekts[1] = nullptr;

	bulletA1= new ThrowAble(glm::mat4(1), "../Models/bulletA.obj",glm::vec3(0,0,0), 1.0);
	bulletB1= new ThrowAble(glm::mat4(1), "../Models/bulletB.obj",glm::vec3(0,0,0), 1.0);
	bulletC1= new ThrowAble(glm::mat4(1), "../Models/bulletC.obj",glm::vec3(0,0,0), 1.0);
	bulletA2= new ThrowAble(glm::mat4(1), "../Models/bulletA.obj",glm::vec3(0,0,0), 1.0);
	bulletB2= new ThrowAble(glm::mat4(1), "../Models/bulletB.obj",glm::vec3(0,0,0), 1.0);
	bulletC2= new ThrowAble(glm::mat4(1), "../Models/bulletC.obj",glm::vec3(0,0,0), 1.0);

	blendet = true;
	frameTime = false;
	wireFrame = false;
	viewFrustumCulling = false;
	TextureSamplingMode = GL_LINEAR;
	MipMappingMode = GL_LINEAR_MIPMAP_LINEAR;
}

void cleanup()
{
	glfwDestroyWindow(window);
	delete cube; cube = nullptr;
	delete boden; boden = nullptr;
	delete boden2; boden2 = nullptr;
	delete sphere; sphere = nullptr;
	delete player1; player1 = nullptr;
	delete player2; player2 = nullptr;

	for(int i = 0; i<WURFANZAHL_MAX; i++)
	{
		delete thrownObjekts[i]; thrownObjekts[i] = nullptr;
	}
	delete thrownObjekts; thrownObjekts = nullptr;

	delete bulletA1; bulletA1 = nullptr;
	delete bulletB1; bulletB1 = nullptr;
	delete bulletC1; bulletC1 = nullptr;
	delete bulletA2; bulletA2 = nullptr;
	delete bulletB2; bulletB2 = nullptr;
	delete bulletC2; bulletC2 = nullptr;

	delete cam; cam = nullptr;
	
	deleteShadowmaps();
	deleteWater();
	cleanupText2D();
	skybox.releaseSkybox();
	

	//delete(particle); particle = nullptr;
	delete frustum1;
	delete frustum2;

	glfwTerminate();
}

void draw(GLuint progHandle, glm::mat4 LPV, Player* playerX, bool inversion) //inversion wird nur fuer die Wasser reflexion gebraucht
{
	if(wireFrame)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	

	if(playerX != nullptr)
	{

	auto proj = glm::perspective(45.0f, (WIDTH/2) / (float)HEIGHT, 0.1f, 200.0f);
	
	auto viewTranslation = glm::lookAt(playerX->playerkamera->getPosition(), 
										playerX->playerkamera->calculateGlobalViewdirection(),
										//playerX->playerkamera->getPosition() + glm::vec3(0,0,1),
										glm::vec3(0,1,0));

	glm::mat4 inv  (1.0, 0.0, 0.0, 0.0,
						0.0, -1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0);
	if(inversion)
	{
		viewTranslation = viewTranslation * inv;
	}
	
	//SKYBOX RENDERING
	
	//glEnable(GL_TEXTURE_2D);
	glUseProgram(skyboxShaders);

	float sunLightAmbientIntensity = 1.0f;
	glm::vec3 sunLightvDirection(0,-1,0);
	glm::vec3 sunLightvColor(1.0f,1.0f,1.0f);
	glm::vec4 vColor(1.0f,1.0f,1.0f,1.0f);

	glm::mat4 model;
	if(playerX->identifier==1)
		glm::mat4 model = glm::translate(glm::mat4(1),(player1->playerkamera->getPosition()));
	if(playerX->identifier==2)
		glm::mat4 model = glm::translate(glm::mat4(1),(player1->playerkamera->getPosition()));
	//glm::mat4 model = glm::mat4(1);
	auto model_location = glGetUniformLocation(skyboxShaders, "modelViewMatrix");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(viewTranslation));
	model_location = glGetUniformLocation(skyboxShaders, "projectionMatrix");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(proj));
	model_location = glGetUniformLocation(skyboxShaders, "normalMatrix");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
	int iLoc = glGetUniformLocation(skyboxShaders, "sunLight.vColor");
	glUniform3fv(iLoc, 1, (GLfloat*)&sunLightvColor);
	iLoc = glGetUniformLocation(skyboxShaders, "sunLight.fAmbientIntensity");
	glUniform1fv(iLoc, 1, &sunLightAmbientIntensity);
	iLoc = glGetUniformLocation(skyboxShaders, "sunLight.vDirection");
	glUniform3fv(iLoc, 1, (GLfloat*)&sunLightvDirection);
	iLoc = glGetUniformLocation(skyboxShaders, "vColor");
	glUniform4fv(iLoc, 1, (GLfloat*)&vColor);

	
	iLoc = glGetUniformLocation(skyboxShaders, "gSampler1");
	glUniform1i(iLoc, 40);
	iLoc = glGetUniformLocation(skyboxShaders, "gSampler2");
	glUniform1i(iLoc, 41);
	iLoc = glGetUniformLocation(skyboxShaders, "gSampler3");
	glUniform1i(iLoc, 42);
	iLoc = glGetUniformLocation(skyboxShaders, "gSampler4");
	glUniform1i(iLoc, 43);
	iLoc = glGetUniformLocation(skyboxShaders, "gSampler5");
	glUniform1i(iLoc, 44);
	iLoc = glGetUniformLocation(skyboxShaders, "gSampler6");
	glUniform1i(iLoc, 45);
	
	skybox.renderSkybox();

	glUseProgram(0);
	glUseProgram(progHandle);
	
	//ENDE DER SKYBOX


	//BODEN
	glm::mat4 LPVM;
	auto view_proj = proj * viewTranslation;// * rot;
	auto view_proj_location = glGetUniformLocation(progHandle, "view");
	glUniformMatrix4fv(view_proj_location, 1, GL_FALSE, glm::value_ptr(view_proj));
	model = boden->mat;
	model_location = glGetUniformLocation(progHandle, "model");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
	LPVM = LPV * model;
	model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

	boden->draw(progHandle, 2, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);
	/*
	model = boden2->mat;
	model_location = glGetUniformLocation(progHandle, "model");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
	LPVM = LPV * model;
	model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

	boden2->draw(progHandle, 2);*/

	model = sphere->mat;
	model_location = glGetUniformLocation(progHandle, "model");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
	LPVM = LPV * model;
	model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

	sphere->draw(progHandle, 3, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);
	

	if(player1 !=nullptr)
	{
		model = player1->playermodell->mat;
		model_location = glGetUniformLocation(progHandle, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		LPVM = LPV * model;
		model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

		player1->drawBody(progHandle, 4, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);

		model = player1->orbiter->mat;
		model_location = glGetUniformLocation(progHandle, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		LPVM = LPV * model;
		model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

		player1->drawOrbiter(progHandle, 4, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);
	}
	if(player2 !=nullptr)
	{
		model = player2->playermodell->mat;
		model_location = glGetUniformLocation(progHandle, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		LPVM = LPV * model;
		model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

		player2->drawBody(progHandle, 4, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);

		model = player2->orbiter->mat;
		model_location = glGetUniformLocation(progHandle, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		LPVM = LPV * model;
		model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

		player2->drawOrbiter(progHandle, 4, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);
	}

	for(int i = 0; i<WURFANZAHL_MAX; i++)
	{
		if(thrownObjekts[i] != nullptr)
		{
			model = thrownObjekts[i]->mat;
			model_location = glGetUniformLocation(progHandle, "model");
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
			LPVM = LPV * model;
			model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

			thrownObjekts[i]->draw(progHandle, 6+i, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);
		}
	}
	//drawBlendet(programHandle, LPV);
	
	}
	
	if(wireFrame)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}

void controlBoundery(int p)
{
	
	if(p == 1)
	{
		glm::vec3 p1midpoint = glm::vec3(player1->playermodell->mat * glm::vec4(player1->playermodell->center,1));
		if( p1midpoint.z < (-1)*maplaenge)
		{
			player1->playerkamera->vorwaerts = false;
		}
		if(p1midpoint.z > maplaenge)
		{
			player1->playerkamera->rueckwaerts = false;
		}
		if(p1midpoint.x < (-1)*mapbreite)
		{
			player1->playerkamera->links = false;
		}
		if(p1midpoint.x > mapbreite)
		{
			player1->playerkamera->rechts = false;
		}
	}
	if(p == 2)
	{
		glm::vec3 p2midpoint = glm::vec3(player2->playermodell->mat * glm::vec4(player2->playermodell->center,1));
		if( p2midpoint.z < (-1)*maplaenge)
		{
			player2->playerkamera->vorwaerts = false;
		}
		if(p2midpoint.z > maplaenge)
		{
			player2->playerkamera->rueckwaerts = false;
		}
		if(p2midpoint.x < (-1)*mapbreite)
		{
			player2->playerkamera->links = false;
		}
		if(p2midpoint.x > mapbreite)
		{
			player2->playerkamera->rechts = false;
		}
	}
	
}

void update(float time_delta)
{

	if(!gameOver){
		if(player1->playerkamera->getPosition().x < 2 && player1->playerkamera->getPosition().x > -2 &&
			player1->playerkamera->getPosition().z < 2 && player1->playerkamera->getPosition().z > -2 &&
			player1->hitpoints < 10 && player1->bonusUsed == false){
			player1->hitpoints = 10;
			player1->ammo1 = 20;
			player1->ammo2 = 10;
			player1->ammo3 = 5;
			player1->bonusUsed = true;
		}

		if(player2->playerkamera->getPosition().x < 2 && player2->playerkamera->getPosition().x > -2 &&
			player2->playerkamera->getPosition().z < 2 && player2->playerkamera->getPosition().z > -2 &&
			player2->hitpoints < 10 && player2->bonusUsed == false){
			player2->hitpoints = 10;
			player2->ammo1 = 20;
			player2->ammo2 = 10;
			player2->ammo3 = 5;
			player2->bonusUsed = true;
		}
	}


	//__________________________________________________________________________

	for(int i = 0; i<WURFANZAHL_MAX; i++)
	{
		if(thrownObjekts[i] != nullptr)
		{
			thrownObjekts[i]->update(time_delta);
			if(i == 0 && player2 != nullptr)
			{
				glm::vec3 midposPlayer = glm::vec3(player2->playermodell->mat * glm::vec4(player2->playermodell->center,1));
				glm::vec3 midposBullet = glm::vec3(thrownObjekts[i]->mat * glm::vec4(thrownObjekts[i]->center,1));
				glm::vec3 temp = midposBullet - midposPlayer;
				float distSqr = dot(temp, temp); //quadratische distanz
				float mindistSqr = (player2->playermodell->hitboxRadius + thrownObjekts[i]->hitboxRadius) * (player2->playermodell->hitboxRadius + thrownObjekts[i]->hitboxRadius);

				if((mindistSqr/4) > distSqr)
				{
					switch(player1->currentBullet){
					case 'A':
						player2->hitpoints -= 1;
						thrownObjekts[i] = nullptr;
						break;
					case 'B':
						player2->hitpoints -= 2;
						thrownObjekts[i] = nullptr;
						break;
					case 'C':
						player2->hitpoints -= 3;
						thrownObjekts[i] = nullptr;
						break;
					}
					player2->playermodell->diffuseMAT -= glm::vec3(0.1,0.1,0.1);
					if(player2->hitpoints <= 0)
					{
						player2->hitpoints = 0;
						player1->playerkamera->resetkeys();
						player2->playerkamera->resetkeys();
						delete(player1);
						player1 = nullptr;
						delete(player2);
						player2 = nullptr;					
						std::cout << "\n\nSPIELER 1 HAT GEWONNEN!" << std::endl;
						gameOver = true;
					}
				}
			}
			if(i == 1 && player1 != nullptr)
			{
				glm::vec3 midposPlayer = glm::vec3(player1->playermodell->mat * glm::vec4(player1->playermodell->center,1));
				glm::vec3 midposBullet = glm::vec3(thrownObjekts[i]->mat * glm::vec4(thrownObjekts[i]->center,1));
				glm::vec3 temp = midposBullet - midposPlayer;
				float distSqr = dot(temp, temp); //quadratische distanz
				float mindistSqr = (player1->playermodell->hitboxRadius + thrownObjekts[i]->hitboxRadius) * (player1->playermodell->hitboxRadius + thrownObjekts[i]->hitboxRadius);

				if((mindistSqr/4) > distSqr)
				{
					switch(player2->currentBullet){
					case 'A':
						player1->hitpoints -= 1;
						thrownObjekts[i] = nullptr;
						break;
					case 'B':
						player1->hitpoints -= 2;
						thrownObjekts[i] = nullptr;
						break;
					case 'C':
						player1->hitpoints -= 3;
						thrownObjekts[i] = nullptr;
						break;
					}
					player1->playermodell->diffuseMAT -= glm::vec3(0.1,0.1,0.1);
					if(player1->hitpoints <= 0)
					{
						player1->hitpoints = 0;
						player1->playerkamera->resetkeys();
						player2->playerkamera->resetkeys();
						delete(player1);
						player1 = nullptr;
						delete(player2);
						player2 = nullptr;					
						std::cout << "\n\nSPIELER 2 HAT GEWONNEN!" << std::endl;
						gameOver = true;
					}
				}
			}
			if(thrownObjekts[i] != nullptr)
			{
				glm::vec3 trans = glm::vec3(thrownObjekts[i]->mat[3].x, thrownObjekts[i]->mat[3].y, thrownObjekts[i]->mat[3].z);
				//if(trans.x > 20 || trans.z > 20 ||trans.x < -20 || trans.z < -20 || trans.y < -10)
				if(trans.y < -10)
				{
					thrownObjekts[i] = nullptr;
				}else{
					//Auf Kollision mit Terrain ueberpruefen
					thrownObjekts[i]->updateHitbox();
					if(thrownObjekts[i]->box.corner.y <= boden->getYposAt(trans.x,trans.z)-1.0f){
						thrownObjekts[i] = nullptr;
					}
				}
			}
		}
	}
}

void initWindow(int argc, char** argv)
{
	
	if (!glfwInit())
	{
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*
	int width, height;
	width = 800;
	height = 600;
	*/
	bool fullscreen = false;

	if( argc == 4)
	{//nur temporär auskommentiert
		/*if((std::stringstream(argv[1]) >>WIDTH).fail() || (std::stringstream(argv[2]) >> HEIGHT).fail() || (std::stringstream(argv[3]) >> fullscreen).fail() )
		{
			std::cout << "Fehlerhafte Uebergabeparameter" << std::endl;
			system("PAUSE");
			exit(EXIT_FAILURE);
		}*/
	}

	GLFWmonitor* monitor = nullptr;
	if(fullscreen)
	{
		monitor = glfwGetPrimaryMonitor();
	}

	window = glfwCreateWindow(WIDTH, HEIGHT, "Throw It", monitor, NULL);

	glfwMakeContextCurrent(window);
	int refresh_rate = 60;
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);


	//glfwSetCursorPosCallback(window, handleMouseMove);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.3,0.3,1,0);
	glEnable (GL_DEPTH_TEST);

	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		//Error
		std::cout << glewGetErrorString(err);
	}

	#if _DEBUG
		// Create a debug OpenGL context or tell your OpenGL library (GLFW, SDL) to do so.
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	#endif

	#if _DEBUG
    // Query the OpenGL function to register your callback function.
    PFNGLDEBUGMESSAGECALLBACKPROC _glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC) wglGetProcAddress("glDebugMessageCallback");
    PFNGLDEBUGMESSAGECALLBACKARBPROC _glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC) wglGetProcAddress("glDebugMessageCallbackARB");
    PFNGLDEBUGMESSAGECALLBACKAMDPROC _glDebugMessageCallbackAMD = (PFNGLDEBUGMESSAGECALLBACKAMDPROC) wglGetProcAddress("glDebugMessageCallbackAMD");

    if(_glDebugMessageCallback != NULL) {
        _glDebugMessageCallback(DebugCallback, NULL);
    }
    else if (_glDebugMessageCallbackARB != NULL) {
        _glDebugMessageCallbackARB(DebugCallback, NULL);
    }
    else if (_glDebugMessageCallbackAMD != NULL) {
        _glDebugMessageCallbackAMD(DebugCallbackAMD, NULL);
    }
 
    // Enable synchronous callback. This ensures that your callback function is called
    // right after an error has occurred. This capability is not defined in the AMD
    // version.
    if ((_glDebugMessageCallback != NULL) || (_glDebugMessageCallbackARB != NULL)) {
        
		(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
	#endif

	glDebugMessageCallback(DebugCallback,NULL);
	glDebugMessageCallbackARB(DebugCallback,NULL);
	glDebugMessageCallbackAMD(DebugCallbackAMD,NULL);
	_glDebugMessageCallback(DebugCallback, NULL);
	_glDebugMessageCallbackARB(DebugCallback, NULL);
	_glDebugMessageCallbackAMD(DebugCallbackAMD, NULL);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

	if(player1 != nullptr && player2 != nullptr)
	{
		if (key == GLFW_KEY_E && action == GLFW_PRESS){
			player1->playerkamera->teleport(glm::vec3(0,5,0));
			std::cout<<"press"<<std::endl;
		}
		if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS){
			player2->playerkamera->teleport(glm::vec3(7.0f,-1.5f,5.0f));
		}
		if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
		}
		if(key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_PRESS)
		{
			switch(player2->currentBullet){
			case 'A':player2->changeBulletTo('B');
				break;
			case 'B':player2->changeBulletTo('C');
				break;
			case 'C':player2->changeBulletTo('A');
				break;
			}
		}
		
		if(key == GLFW_KEY_KP_0 && action == GLFW_PRESS)
		{
			shoot(2,player2);
			
		}

		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case 'W':
				player1->playerkamera->rueckwaerts = false;
				player1->playerkamera->vorwaerts = true;
				break;
			case 'S':
				player1->playerkamera->vorwaerts = false;
				player1->playerkamera->rueckwaerts = true;
				break;
			case 'A':
			    player1->playerkamera->rechts = false;
				player1->playerkamera->links = true;
				break;
			case 'D':
			    player1->playerkamera->links = false;
				player1->playerkamera->rechts = true;
				break;
			case GLFW_KEY_DOWN:
				player2->playerkamera->rueckwaerts = true;
				player2->playerkamera->vorwaerts = false;
				break;
			case GLFW_KEY_UP:
				player2->playerkamera->vorwaerts = true;
				player2->playerkamera->rueckwaerts = false;
				break;
			case GLFW_KEY_RIGHT:
			    player2->playerkamera->rechts = true;
				player2->playerkamera->links = false;
				break;
			case GLFW_KEY_LEFT:
			    player2->playerkamera->links = true;
				player2->playerkamera->rechts = false;
				break;
			case GLFW_KEY_KP_4:
				player2->playerkamera->turnLeftNumpad = true;
				player2->playerkamera->turnRightNumpad = false;
				break;
			case GLFW_KEY_KP_6:
				player2->playerkamera->turnRightNumpad = true;
				player2->playerkamera->turnLeftNumpad = false;
				break;
			case GLFW_KEY_KP_8:
				player2->playerkamera->turnUpNumpad = true;
				player2->playerkamera->turnDownNumpad = false;
				break;
			case GLFW_KEY_KP_5:
				player2->playerkamera->turnDownNumpad = true;
				player2->playerkamera->turnUpNumpad = false;
				break;
			default:
			//nichts
			break;
		}
		if(key == GLFW_KEY_F9 && action == GLFW_PRESS)
		{
			if (blendet)
			{
				blendet = false;
				std::cout<< "Transparenz ist jetzt aus"<<std::endl;
			}else{
				blendet = true;
				std::cout<< "Transparenz ist jetzt an"<<std::endl;
			}
		}
		if(key == GLFW_KEY_F2 && action == GLFW_PRESS)
		{
			if (frameTime)
			{
				frameTime = false;
				std::cout<< "FrameTime aus"<<std::endl;
			}else{
				frameTime = true;
				std::cout<< "FrameTime an"<<std::endl;
			}
		}
		if(key == GLFW_KEY_F3 && action == GLFW_PRESS)
		{
			if (wireFrame)
			{
				wireFrame = false;
				std::cout<< "WireFrame aus"<<std::endl;
			}else{
				wireFrame = true;
				std::cout<< "WireFrame an"<<std::endl;
			}
		}
		if(key == GLFW_KEY_F4 && action == GLFW_PRESS)
		{
			if(TextureSamplingMode == GL_LINEAR)
			{
				TextureSamplingMode = GL_NEAREST;
				std::cout<< "Textur sampling nearest"<<std::endl;
			}else{
				TextureSamplingMode = GL_LINEAR;
				std::cout<< "Textur sampling linear"<<std::endl;
			}
		}
		if(key == GLFW_KEY_F5 && action == GLFW_PRESS)
		{
			if(MipMappingMode == GL_LINEAR_MIPMAP_LINEAR)
			{
				MipMappingMode = GL_NONE;
				std::cout<< "Mipmapping aus"<<std::endl;
			}else{
				if(MipMappingMode == GL_NONE)
				{
					MipMappingMode = GL_NEAREST_MIPMAP_NEAREST;
					std::cout<< "Mipmapping nearest"<<std::endl;
				}else{
					MipMappingMode = GL_LINEAR_MIPMAP_LINEAR;
					std::cout<< "Mipmapping bilinear"<<std::endl;
				}
			}
		}
		if(key == GLFW_KEY_F8 && action == GLFW_PRESS){
			if(!viewFrustumCulling) viewFrustumCulling = true;
			else viewFrustumCulling = false;
		}		if(key == GLFW_KEY_F6 && action == GLFW_PRESS)
		{
			if (particlecount)
			{
				particlecount = false;
				std::cout<< "Anzahl Partikel aus"<<std::endl;
			}else{
				particlecount = true;
				std::cout<< "Anzahl Partikel an"<<std::endl;
			}
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case 'W':
			player1->playerkamera->vorwaerts = false;
			break;
		case 'S':
			player1->playerkamera->rueckwaerts = false;
			break;
		case 'A':
			player1->playerkamera->links = false;
			break;
		case 'D':
			player1->playerkamera->rechts = false;
			break;
		case GLFW_KEY_DOWN:
			player2->playerkamera->rueckwaerts = false;
			break;
		case GLFW_KEY_UP:
			player2->playerkamera->vorwaerts = false;
			break;
		case GLFW_KEY_RIGHT:
			player2->playerkamera->rechts = false;
			break;
		case GLFW_KEY_LEFT:
			player2->playerkamera->links = false;
			break;
		case GLFW_KEY_KP_4:
			player2->playerkamera->turnLeftNumpad = false;
			break;
		case GLFW_KEY_KP_6:
			player2->playerkamera->turnRightNumpad = false;
			break;
		case GLFW_KEY_KP_8:
			player2->playerkamera->turnUpNumpad = false;
			break;
		case GLFW_KEY_KP_5:
			player2->playerkamera->turnDownNumpad = false;
			break;
		default:
			// nichts
			break;
		}
	}
	}
}

void handleMouseMove(GLFWwindow* w)//Maus
{
	//printf("Rotation : x = %.2f / y = %.2f / z = %.2f \n", player1->playerkamera->getXRot(),player1->playerkamera->getYRot(),player1->playerkamera->getZRot());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	player1->playerkamera->mousemove(xpos, ypos);

	glfwSetCursorPos(window,WIDTH/2,HEIGHT/2);
	
}

glm::mat4 initShadowMapsAndLight()
{	
	glGenTextures(1, &ztex);
	glBindTexture(GL_TEXTURE_2D, ztex);
	// 512:512 standart. Wenn zu schlecht auf 1024:1024 erhoehen
	glTexImage2D(GL_TEXTURE_2D, 0, 
					GL_DEPTH_COMPONENT24, 512, 512, 0,
					GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_COMPARE_FUNC, GL_LESS);


	//Framebuffer

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT, ztex, 0);
	glDrawBuffer(GL_NONE); //nur Tiefenwerte
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(EXIT_FAILURE);

	//lichtquelle anbringen(noch unsichtbar)
	//DirectionalLight* light = new DirectionalLight(glm::vec3(10.0f, 7.0f, 2.0f), //position
	light = new DirectionalLight(glm::vec3(10.0f, 7.0f, 2.0f), //position
												glm::lookAt(
												glm::vec3(10.0f, 7.0f, 4.0f), //position
												glm::vec3(0,0,0),   //Zentrum der Blickrichtung
												glm::vec3(0,1,0)),	//welche Achse ist oben
												glm::perspective(45.0f, WIDTH / (float)HEIGHT,0.1f,25.0f));
												//glm::ortho<float>(-30,30,-30,30,-30,30));
	
	/*DirectionalLight* light2 = new DirectionalLight(glm::vec3(40.0f, 7.0f, 32.0f), //position
												glm::lookAt(
												glm::vec3(40.0f, 7.0f, 34.0f), //position
												glm::vec3(30,0,30),   //Zentrum der Blickrichtung
												glm::vec3(0,1,0)),	//welche Achse ist oben
												glm::perspective(45.0f, WIDTH / (float)HEIGHT,0.1f,25.0f));
												//glm::ortho<float>(-30,30,-30,30,-30,30));
												*/

	//GLuint zShaders = loader.LoadShaders("../Shader/vertexZShader.vert", "../Shader/fragmentZShader.frag");
	zShaders = loader.LoadShaders("../Shader/vertexZShader.vert", "../Shader/fragmentZShader.frag");
	glm::mat4 PV = light->projection * light->view * glm::mat4(1);
	//glm::mat4 PV2 = light2->projection * light2->view * glm::mat4(1);
	return PV;
}

void renderShadowmaps(glm::mat4 PV)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(5.0f, 0.f);

		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		glViewport(0,0,512,512);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(zShaders);
		

		//CUBE
		auto modelmat = cube->mat;
		glm::mat4 M_Vl_Pl_Matrix = PV * modelmat;
		auto model_location = glGetUniformLocation(zShaders, "M_Vl_Pl_Matrix");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
		cube->draw(zShaders, 1, MipMappingMode, TextureSamplingMode);

		//BODEN
		modelmat = boden->mat;
		M_Vl_Pl_Matrix = PV * modelmat;
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
		boden->draw(zShaders, 2, MipMappingMode, TextureSamplingMode);

		//BODEN2
		modelmat = boden2->mat;
		M_Vl_Pl_Matrix = PV * modelmat;
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
		boden2->draw(zShaders, 8, MipMappingMode, TextureSamplingMode);

		//SPHERE
		modelmat = sphere->mat;
		M_Vl_Pl_Matrix = PV * modelmat;
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
		sphere->draw(zShaders, 3, MipMappingMode, TextureSamplingMode);

		//PLAYERS
		if(player1 != nullptr)
		{
			modelmat = player1->playermodell->mat;
			M_Vl_Pl_Matrix = PV * modelmat;
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
			player1->drawBody(zShaders, 4, MipMappingMode, TextureSamplingMode);

			modelmat = player1->orbiter->mat;
			M_Vl_Pl_Matrix = PV * modelmat;
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
			player1->drawOrbiter(zShaders, 4, MipMappingMode, TextureSamplingMode);
		}
		if(player2 != nullptr)
		{
			modelmat = player2->playermodell->mat;
			M_Vl_Pl_Matrix = PV * modelmat;
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
			player2->drawBody(zShaders, 5, MipMappingMode, TextureSamplingMode);

			modelmat = player2->orbiter->mat;
			M_Vl_Pl_Matrix = PV * modelmat;
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
			player2->drawOrbiter(zShaders, 4, MipMappingMode, TextureSamplingMode);
		}

		//BULLETS
		for(int i = 0; i<WURFANZAHL_MAX; i++)
		{
			if(thrownObjekts[i] != nullptr)
			{
				modelmat = thrownObjekts[i]->mat;
				M_Vl_Pl_Matrix = PV * modelmat;
				glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(M_Vl_Pl_Matrix));
				thrownObjekts[i]->draw(zShaders, 4+i, MipMappingMode, TextureSamplingMode);
			}
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
		
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void deleteShadowmaps()
{
	glDeleteProgram(zShaders);
	free(light);
	glDeleteBuffers(1, &fb);
	glDeleteTextures(1, &ztex);
}

void initWaterRecources()
{
	for(int i = 0; i < 2; i++)
	{
	//-.-.-.-.-.-.-Reflexionstextur und Reflexionsframebuffer-.-.-.-.-.-.-.-
	glGenTextures(1, &reflexiontex[i]);
	glBindTexture(GL_TEXTURE_2D, reflexiontex[i]);
	// 512:512 standart. Wenn zu schlecht auf 1024:1024 erhoehen
	glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	//Framebuffer
	glGenFramebuffers(1, &fb_reflexion[i]);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_reflexion[i]);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0, reflexiontex[i], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(EXIT_FAILURE);
	//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-


	//-.-.-.-.-.-.-Refractionstextur und Refracionsframebuffer-.-.-.-.-.-
	glGenTextures(1, &refractiontex[i]);
	glBindTexture(GL_TEXTURE_2D, refractiontex[i]);
	// 512:512 standart. Wenn zu schlecht auf 1024:1024 erhoehen
	glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	//Framebuffer
	glGenFramebuffers(1, &fb_refraction[i]);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_refraction[i]);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0, refractiontex[i], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(EXIT_FAILURE);

	//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
	}
	waterShaders = loader.LoadShaders("../Shader/WaterVertexShader.vert", "../Shader/WaterFragmentShader.frag");

	water = new Waterplane(maplaenge,mapbreite,waterShaders);

	Texture texloader;
	WaterNormalMap  = texloader.loadTexture("../Models/waterNormals.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	WaterDuDvMap  = texloader.loadTexture("../Models/waterDuDv.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

void renderWaterResources(GLuint progHandle, glm::mat4 LPV, Player* playerX, int playernumber)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderWaterReflexion(progHandle, LPV, playerX, playernumber);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderWaterRefraction(progHandle, LPV, playerX, playernumber);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderWaterReflexion(GLuint progHandle, glm::mat4 LPV, Player* playerX, int playernumber)
{
	//Szene wird gespiegelt und alles was unter Wasser waere wird abgeschnitten
	glBindFramebuffer(GL_FRAMEBUFFER, fb_reflexion[playernumber]);
	glViewport(0,0,512,512);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_PLANE0);

	glm::vec4 cutPlane = glm::vec4(0.0,1.0,0.0,0.0);
	auto plane_location = glGetUniformLocation(progHandle, "u_plane0");
	glUniform4f(plane_location, cutPlane.x, cutPlane.y, cutPlane.z, cutPlane.w);

	draw(progHandle, LPV, playerX,true);

	glDisable(GL_CLIP_PLANE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWaterRefraction(GLuint progHandle, glm::mat4 LPV, Player* playerX, int playernumber)
{
	//Hier wird Szene normal gerendert aber alles ueberhalb des Wassers abgeschnitten
	glBindFramebuffer(GL_FRAMEBUFFER, fb_refraction[playernumber]);
	glViewport(0,0,512,512);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_PLANE0);
	
	glm::vec4 cutPlane = glm::vec4(0.0,-1.0,0.0,0.0);
	auto plane_location = glGetUniformLocation(progHandle, "u_plane0");
	glUniform4f(plane_location, cutPlane.x, cutPlane.y, cutPlane.z, cutPlane.w);

	draw(progHandle, LPV, playerX,false);

	glDisable(GL_CLIP_PLANE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawWater(Player* playerX, double delta_t, int textureIndex, int playernumber)
{
	glUseProgram(waterShaders);

	auto proj = glm::perspective(45.0f, (WIDTH/2) / (float)HEIGHT, 0.1f, 100.0f);
	//auto viewTranslation = glm::translate(glm::mat4(1.0f), playerX->playerkamera->getPosition());
	
	auto viewTranslation = glm::lookAt(playerX->playerkamera->getPosition(), 
		playerX->playerkamera->calculateGlobalViewdirection(),
									   glm::vec3(0,1,0));
	
	//viewTranslation = glm::rotate(viewTranslation, glm::radians((float)playerX->playerkamera->getYRot()), glm::vec3(0,-1,0));

	auto mpv = water->getmat() *  proj * viewTranslation ;

	//-.-.-.-.-.-.-.-uniforms vertex shader-.-.-.-.-.-.-.-.-.-.-.-.-
	auto mvp_location = glGetUniformLocation(waterShaders, "mvpMatrix");
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mpv));

	auto time_location = glGetUniformLocation(waterShaders, "uniTime");
	glUniform1f(time_location, (float) glfwGetTime());

	auto model_location = glGetUniformLocation(waterShaders, "model");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(water->getmat()));

	auto view_location = glGetUniformLocation(waterShaders, "view");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(viewTranslation));

	auto ligthpos_location = glGetUniformLocation(waterShaders, "lightPOS");
	glUniform3f(ligthpos_location, light->pos.x, light->pos.y, light->pos.z);

	//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	//-.-.-.-.-.-.-.-uniforms fragment shader-.-.-.-.-.-.-.-.-.-.-.-.-
	auto baseColor_location = glGetUniformLocation(waterShaders, "vBaseColorValue");
	glUniform4f(baseColor_location, 0.0f, 0.0f, 1.0f, 0.0f);

	auto viewPos_location = glGetUniformLocation(waterShaders, "ViewPos");
	glUniform3f(viewPos_location, playerX->playerkamera->getXPos(), playerX->playerkamera->getYPos(), playerX->playerkamera->getZPos());

	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, reflexiontex[playernumber]);
	auto reflectex_location = glGetUniformLocation(waterShaders, "waterReflection");
	glUniform1i(reflectex_location, textureIndex);

	glActiveTexture(GL_TEXTURE0 + textureIndex + 1);
	glBindTexture(GL_TEXTURE_2D, refractiontex[playernumber]);
	auto refracttex_location = glGetUniformLocation(waterShaders, "waterRefraction");
	glUniform1i(refracttex_location, textureIndex + 1);

	glActiveTexture(GL_TEXTURE0 + textureIndex + 2);
	glBindTexture(GL_TEXTURE_2D, WaterNormalMap);
	auto normaltex_location = glGetUniformLocation(waterShaders, "normalMap");
	glUniform1i(normaltex_location, textureIndex + 2);

	glActiveTexture(GL_TEXTURE0 + textureIndex + 3);
	glBindTexture(GL_TEXTURE_2D, WaterDuDvMap);
	auto dudvtex_location = glGetUniformLocation(waterShaders, "dudv");
	glUniform1i(dudvtex_location, textureIndex + 3);

	//auto lightdir_location = glGetUniformLocation(waterShaders, "LightDirection");
	//glUniform3f(lightdir_location, -0.5f, 0.5f, 0.0f);
	//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

	water->draw();
	glUseProgram(0);

}

void deleteWater()
{
	for(int i = 0; i<2;i++)
	{
	glDeleteBuffers(1, &fb_reflexion[i]);
	glDeleteTextures(1, &reflexiontex[i]);

	glDeleteBuffers(1, &fb_refraction[i]);
	glDeleteTextures(1, &refractiontex[i]);
	}
	
	glDeleteTextures(1, &WaterNormalMap);
	glDeleteTextures(1, &WaterDuDvMap);

	glDeleteProgram(waterShaders);

	delete water; water = nullptr;
}

void shoot(int player, Player* _player){

	if(thrownObjekts[player-1] == nullptr)
		{
			glm::mat4 matrix = glm::mat4(_player->playermodell->mat);
			glm::vec3 richtung;
			if (player==1){
				richtung = player1->playerkamera->calculateLocalViewdirection();
			}
			if (player==2){
				richtung = player2->playerkamera->calculateLocalViewdirection();
			}
			
			if(player==1 && thrownObjekts[player-1] == nullptr){
				switch(_player->currentBullet){
				case 'A':
					if(player1->ammo1>0){
						thrownObjekts[0] = bulletA1;
						player1->ammo1--;
					}
					break;
				case 'B':
					if(player1->ammo2>0){
						thrownObjekts[0] = bulletB1;
						player1->ammo2--;
					}
					break;
				case 'C':
					if(player1->ammo3>0){
						thrownObjekts[0] = bulletC1;
						player1->ammo3--;
					}
					break;
				}
				if(thrownObjekts[player-1] != nullptr){
					thrownObjekts[player-1]->direction = richtung;
					thrownObjekts[player-1]->setForce(_player->shootingForce);
					thrownObjekts[player-1]->mat = matrix;
					thrownObjekts[player-1]->calculateThrowParameters();
				}
			}

			if(player==2 && thrownObjekts[player-1] == nullptr){
				switch(_player->currentBullet){
				case 'A':
					if(player2->ammo1>0){
						thrownObjekts[1] = bulletA2;
						player2->ammo1--;
					}
					break;
				case 'B':
					if(player2->ammo2>0){
						thrownObjekts[1] = bulletB2;
						player2->ammo2--;
					}
					break;
				case 'C':
					if(player2->ammo3>0){
						thrownObjekts[1] = bulletC2;
						player2->ammo3--;
					}
					break;
				}
				if(thrownObjekts[player-1] != nullptr){
					thrownObjekts[player-1]->direction = richtung;
					thrownObjekts[player-1]->setForce(_player->shootingForce);
					thrownObjekts[player-1]->mat = matrix;
					thrownObjekts[player-1]->calculateThrowParameters();
				}
			}		
		}
}
void drawBlendet(GLuint progHandle, glm::mat4 LPV)
{
	glUseProgram(progHandle);
	if(blendet)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}	
	//-.-.-.-.-.-draw here transparent objects-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

		glm::mat4 LPVM;
		auto model = cube->mat;
		auto model_location = glGetUniformLocation(progHandle, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		LPVM = LPV * model;
		model_location = glGetUniformLocation(progHandle, "M_Vl_Pl_L_Matrix");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(LPVM));

		cube->draw(progHandle, 1, MipMappingMode, TextureSamplingMode, frustum1, viewFrustumCulling);

	//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
	if (blendet)
	{
		glDisable(GL_BLEND);
	}
	glUseProgram(0);
}

static void APIENTRY DebugCallbackAMD(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
    std::string error = FormatDebugOutput(category, category, id, severity, message);
    std::cout << error << std::endl;
}
 
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
    std::string error = FormatDebugOutput(source, type, id, severity, message);
    std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
    std::stringstream stringStream;
    std::string sourceString;
    std::string typeString;
    std::string severityString;
 
    // The AMD variant of this extension provides a less detailed classification of the error,
    // which is why some arguments might be "Unknown".
    switch (source) {
        case GL_DEBUG_CATEGORY_API_ERROR_AMD:
        case GL_DEBUG_SOURCE_API: {
            sourceString = "API";
            break;
        }
        case GL_DEBUG_CATEGORY_APPLICATION_AMD:
        case GL_DEBUG_SOURCE_APPLICATION: {
            sourceString = "Application";
            break;
        }
        case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
            sourceString = "Window System";
            break;
        }
        case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
        case GL_DEBUG_SOURCE_SHADER_COMPILER: {
            sourceString = "Shader Compiler";
            break;
        }
        case GL_DEBUG_SOURCE_THIRD_PARTY: {
            sourceString = "Third Party";
            break;
        }
        case GL_DEBUG_CATEGORY_OTHER_AMD:
        case GL_DEBUG_SOURCE_OTHER: {
            sourceString = "Other";
            break;
        }
        default: {
            sourceString = "Unknown";
            break;
        }
    }
 
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: {
            typeString = "Error";
            break;
        }
        case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
            typeString = "Deprecated Behavior";
            break;
        }
        case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
            typeString = "Undefined Behavior";
            break;
        }
        case GL_DEBUG_TYPE_PORTABILITY_ARB: {
            typeString = "Portability";
            break;
        }
        case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
        case GL_DEBUG_TYPE_PERFORMANCE: {
            typeString = "Performance";
            break;
        }
        case GL_DEBUG_CATEGORY_OTHER_AMD:
        case GL_DEBUG_TYPE_OTHER: {
            typeString = "Other";
            break;
        }
        default: {
            typeString = "Unknown";
            break;
        }
    }
 
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            severityString = "High";
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM: {
            severityString = "Medium";
            break;
        }
        case GL_DEBUG_SEVERITY_LOW: {
            severityString = "Low";
            break;
        }
        default: {
            severityString = "Unknown";
            break;
        }
    }
 
    stringStream << "OpenGL Error: " << msg;
    stringStream << " [Source = " << sourceString;
    stringStream << ", Type = " << typeString;
    stringStream << ", Severity = " << severityString;
    stringStream << ", ID = " << id << "]";
 
    return stringStream.str();
}

void initParticle()
{
	particle = new Partikelsystem();

	particle->setProperties( 
      glm::vec3(5.0f, 5.0f, 6.0f), // Where the particles are generated
      glm::vec3(-5.0f, 0.0f, -5.0f), // Minimal velocity
      //glm::vec3(5, 20, 5), // Maximal velocity
	  glm::vec3(5.0f, 20.0f, 5.0f),
      //glm::vec3(0.0f, -5.0f, 0.0f), // Gravity force applied to particles
	  glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.5f, 1.0f), // Color (light blue)
      1.5f, // Minimum lifetime in seconds
      3.0f, // Maximum lifetime in seconds
	  //0.75f, // Rendered size
	  0.25f,
	  0.02f, // Spawn every 0.05 seconds
	  30); // And spawn 30 particles
}

void drawParticle(float time_delta, Player* playerx)
{
	auto proj = glm::perspective(45.0f, WIDTH / (float)HEIGHT,0.1f,25.0f);

	particle->setMatrices(&proj,/* glm::vec3(0.0,0.5,1.0),*/playerx->playerkamera->getPosition(),/* playerx->playerkamera->getRotation()*/glm::vec3(0.0,0.0,-1.0), glm::vec3(0,1,0)); 


	particle->draw(time_delta);
	if(particlecount)
	{
		std::cout<<particle->getNumParticles()<<std::endl;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(player1 != nullptr){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		player1->shootingForceIncreasing = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        shoot(1,player1);
		player1->shootingForce = player1->minShootingForce;
		player1->shootingForceIncreasing = false;
	}
	
	if(button == GLFW_MOUSE_BUTTON_RIGHT &&  action == GLFW_PRESS)
		{
			switch(player1->currentBullet){
			case 'A':player1->changeBulletTo('B');
				break;
			case 'B':player1->changeBulletTo('C');
				break;
			case 'C':player1->changeBulletTo('A');
				break;
			}
		}
	}
}

bool collide(Modell* a, Modell* b)
{
	/*if(glm::distance(a->box.corner,b->box.corner) > std::max(a->box.x,std::max(a->box.y,std::max(a->box.z,std::max(b->box.x,std::max(b->box.y,b->box.z)))))){
		return false;
	}
	if(a->box.corner.x > b->box.corner.x && a->box.corner.x < b->box.corner.x+b->box.x &&
		a->box.corner.y > b->box.corner.y && a->box.corner.y < b->box.corner.y+b->box.y &&
		a->box.corner.z > b->box.corner.z && a->box.corner.z < b->box.corner.z+b->box.z)*/
	if(glm::distance(a->box.corner+glm::vec3(a->box.x/2,a->box.y/2,a->box.z/2),b->box.corner+glm::vec3(b->box.x/2,b->box.y/2,b->box.z/2)) <
		std::max(a->box.x,std::max(a->box.y,std::max(a->box.z,std::max(b->box.x,std::max(b->box.y,b->box.z))))))
		return true;
	return false;

}

void initSkybox(){

	Shaderloader* skyloader = new Shaderloader();
	skyboxShaders = skyloader->LoadShaders("../Shader/cubeShader.vert","../Shader/cubeShader.frag");
	skybox.loadSkybox(100.0f,"../Models/skybox_textures/", "jajlands1_ft.jpg", "jajlands1_bk.jpg", "jajlands1_lf.jpg", "jajlands1_rt.jpg", "jajlands1_up.jpg", "jajlands1_dn.jpg");

}