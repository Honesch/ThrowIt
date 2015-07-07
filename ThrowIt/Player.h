#pragma once
#include "Modell.h"
#include "Kamera.h"
#include "ThrowAble.h"
#include "FrustumG.h"

class Player
{
public:
	Player(Modell* mod, Modell* _orbiter, Kamera* cam, int _identifier, char _startBullet, double _minShootingForce, double _maxShootingForce);
	~Player(void);

	Modell* playermodell;
	Modell* orbiter;
	Kamera* playerkamera;

	int ammo1, ammo2, ammo3;
	bool bonusUsed;
	double hitpoints;
	int identifier; //1 oder 2 
	char currentBullet;
	double shootingForce;
	double currentOrbiterAngle;
	double minShootingForce;
	double maxShootingForce;
	bool shootingForceIncreasing;
	double shootingForceThreshold1;
	double shootingForceThreshold2;
	double shootingForceThreshold3;
	double shootingForceThreshold4;
	double shootingForceThreshold5;
	double shootingForceThreshold6;
	int shootingForceStage;
	int x;

	void drawBody(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullingEnabled);
	void drawBody(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter);
	void drawOrbiter(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter, FrustumG* frustum, bool cullingEnabled);
	void drawOrbiter(GLuint handle, int textureI, GLuint minificationFilter, GLuint magnificationFilter);
	void teleport(glm::vec3 destination);
	void updateBodyToCamRelation();
	void updateOrbiterToCamRelation(float time_delta);
	void updateOrbiterToCamRelation(float time_delta, FrustumG* frustum, Player* otherPlayer, Modell* object);//just for debugging
	void changeBulletTo(char _bulletType); //'A', 'B' oder 'C'
	void setShootingForce(double _force);
	void increaseShootingForce(double increase);
	void updateShootingForce(double delta);
	void calculateShootingForceThresholds(double min, double max);
	int getShootingForceStage();

private:
	
};

