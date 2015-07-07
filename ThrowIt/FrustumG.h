#pragma once
#include <glm\glm.hpp>
#include "Plane.h"

class Plane;

#include "AABox.h"

class AABox;




class FrustumG 
{
private:

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};


public:

	static enum {OUTSIDE, INTERSECT, INSIDE};

	Plane pl[6];


	glm::vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle,tang;
	float nw,nh,fw,fh;

	FrustumG::FrustumG();
	FrustumG::~FrustumG();

	void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD);
	void FrustumG::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u);
	int FrustumG::pointInFrustum(glm::vec3 &p);
	int FrustumG::sphereInFrustum(glm::vec3 &p, float raio);
	int FrustumG::boxInFrustum(AABox &b);

	void FrustumG::drawPoints();
	void FrustumG::drawLines();
	void FrustumG::drawPlanes();
	void FrustumG::drawNormals();
};

