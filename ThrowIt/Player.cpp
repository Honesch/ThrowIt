#pragma once
#include "Player.h"
#include "FrustumG.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


Player::Player(Modell* mod, Modell* _orbiter, Kamera* cam, int _identifier, char _startBullet, double _minShootingForce, double _maxShootingForce)
{
	playermodell = mod;
	playerkamera = cam;
	orbiter = _orbiter;
	glm::mat4 scalematrix = glm::scale(orbiter->mat,glm::vec3(0.2f,0.2f,0.2f));
	orbiter->mat = scalematrix;

	identifier = _identifier;
	hitpoints = 10.0f;
	currentBullet=_startBullet;	
	currentOrbiterAngle = 0;
	ammo1 = 20;
	ammo2 = 10;
	ammo3 = 5;
	bonusUsed = false;
	minShootingForce = _minShootingForce;
	maxShootingForce = _maxShootingForce;
	shootingForce = minShootingForce;
	shootingForceIncreasing = false;
	calculateShootingForceThresholds(_minShootingForce,_maxShootingForce);
	shootingForceStage = 0;
	x=1; //DEBUGGING
}

Player::~Player(void)
{
	delete orbiter; orbiter = nullptr;
	delete playermodell; playermodell = nullptr;
	delete playerkamera; playerkamera = nullptr;
}

void Player::drawBody(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullgingEnabled)
{
	playermodell->draw(handle, textureI, minificationFilter, magnificationFilter, frustum, cullgingEnabled);
}
void Player::drawBody(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter)
{
	playermodell->draw(handle, textureI, minificationFilter, magnificationFilter);
}

void Player::drawOrbiter(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullgingEnabled)
{
	orbiter->draw(handle, textureI, minificationFilter, magnificationFilter, frustum, cullgingEnabled);
}
void Player::drawOrbiter(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter)
{
	orbiter->draw(handle, textureI, minificationFilter, magnificationFilter);
}


void Player::updateBodyToCamRelation()
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians((float)playerkamera->getYRot()*(-1)), glm::vec3(0,1,0));
	/*glm::mat4 translation = glm::translate(rotation, glm::vec3((playerkamera->getXPos()) * (-1) - 0.2f, 
										(playerkamera->getYPos()) * (-1) - 0.3f,
										(playerkamera->getZPos()) * (-1) - 0.4f));*/
	glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(playerkamera->getXPos(), playerkamera->getYPos() - 0.3f, playerkamera->getZPos()));

	playermodell->mat = translation*rotation;
}

void Player::updateOrbiterToCamRelation(float time_delta){

	float camXpos = playerkamera->getXPos();
	float camYpos = playerkamera->getYPos();
	float camZpos = playerkamera->getZPos();

	glm::vec3 orbiterDir ((float)cos(currentOrbiterAngle),0.0f,(float)sin(currentOrbiterAngle));

	//glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians((float)playerkamera->getYRot()), glm::vec3(0,1,0));
	glm::mat4 newPosition = glm::translate(glm::mat4(1),glm::vec3( camXpos+(float)cos(currentOrbiterAngle)*0.5f, camYpos-0.5f, camZpos+(float)sin(currentOrbiterAngle)*0.5f));
	
	currentOrbiterAngle += 3*time_delta;
	if(currentOrbiterAngle>2*M_PI) currentOrbiterAngle=0;

	orbiter->mat = newPosition;	
	
}

//DEBUG
void Player::updateOrbiterToCamRelation(float time_delta, FrustumG* frustum, Player* otherPlayer, Modell* object){

		//DEBUG
		if(identifier==1){
			x++;
			if(x==9) x=1;

			/*if(x==1)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(0,object->box.y,0));
			else if(x==2)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(object->box.x,object->box.y,0));
			else if(x==3)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(object->box.x,object->box.y,object->box.z));
			else if(x==4)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(0,object->box.y,object->box.z));
			else if(x==5)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(0,0,0));
			else if(x==6)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(object->box.x,0,0));
			else if(x==7)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(object->box.x,0,object->box.z));
			else if(x==8)
				orbiter->mat =glm::translate(glm::mat4(1), object->box.corner+glm::vec3(0,0,object->box.z));*/

			if(x==1)
				orbiter->mat =glm::translate(glm::mat4(1),frustum->fbl);
			else if(x==2)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->fbr);
			else if(x==3)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->ftl);
			else if(x==4)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->ftr);
			else if(x==5)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->nbl);
			else if(x==6)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->nbr);
			else if(x==7)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->ntl);
			else if(x==8)
				orbiter->mat =glm::translate(glm::mat4(1), frustum->ntr);

			/*printf("Playercam: %.1f %.1f %.1f / Playermodell: %.1f %.1f %.1f\n",
				otherPlayer->playerkamera->getXPos(),
				otherPlayer->playerkamera->getYPos(),
				otherPlayer->playerkamera->getZPos(),
				otherPlayer->playermodell->mat[3].x,
				otherPlayer->playermodell->mat[3].y,
				otherPlayer->playermodell->mat[3].z);*/
		
		
		}
		int status = frustum->pointInFrustum(otherPlayer->playerkamera->getPosition());
		
		/*if(status==0) printf("outside\n");
		if(status==1) printf("intersect\n");
		if(status==2) printf("inside\n");*/
		
}

void Player::teleport(glm::vec3 destination)
{
	playerkamera->teleport(destination);
}

void Player::changeBulletTo(char _bulletType){
	currentBullet=_bulletType;
}

void Player::setShootingForce(double _force){
	shootingForce=_force;
}

void Player::increaseShootingForce(double increase){
	shootingForce += increase;
	if(shootingForce > maxShootingForce) shootingForce = maxShootingForce;
}

void Player::updateShootingForce(double delta){
	if(shootingForceIncreasing) increaseShootingForce(7*delta);
	shootingForceStage = getShootingForceStage();
	//printf("Stage %i / %.1f \n",shootingForceStage,(float)shootingForce);
}

void Player::calculateShootingForceThresholds(double min, double max){
	double diff = max-min;
	double step = diff / 5.0;
	shootingForceThreshold1 = min;
	shootingForceThreshold2 = min + step;
	shootingForceThreshold3 = min + 2*step;
	shootingForceThreshold4 = min + 3*step;
	shootingForceThreshold5 = min + 4*step;
	shootingForceThreshold6 = max;
}
int Player::getShootingForceStage(){
	int stage = 0;
	if(shootingForce == minShootingForce) return stage;
	if(shootingForce > shootingForceThreshold1){
		if(shootingForce >= shootingForceThreshold2){
			if(shootingForce >= shootingForceThreshold3){
				if(shootingForce >= shootingForceThreshold4){
					if(shootingForce >= shootingForceThreshold5){
						return 5;
					}
					return 4;
				}
				return 3;
			}
			return 2;
		}
		return 1;
	}
	return stage; //kommt nie hier her
		
}

