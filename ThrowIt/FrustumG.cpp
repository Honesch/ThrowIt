#pragma once
#include "FrustumG.h"
#include <math.h>
#include <stdio.h>

#define ANG2RAD 3.14159265358979323846/180.0
FrustumG::FrustumG() {}

FrustumG::~FrustumG() {}

void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD) {

	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	tang = (float)tan(angle* ANG2RAD * 0.5);
	nh = nearD * tang;
	nw = nh * ratio; 
	fh = farD  * tang;
	fw = fh * ratio;

}


void FrustumG::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) {

	glm::vec3 dir,nc,fc,X,Y,Z;

	// compute the Z axis of camera
	// this axis points in the opposite direction from
	// the looking direction
	
	//Z = p - l;				//original
	Z = l * glm::vec3(-1,-1,-1);  //meine veraenderung
	glm::normalize(Z);
	
	// X axis of camera with given "up" vector and Z axis
	X = glm::cross(u,Z);
	glm::normalize(X);
	
	// the real "up" vector is the cross product of Z and X
	Y = glm::cross(Z,X);

	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	pl[TOP].set3Points(ntr,ntl,ftl);
	pl[BOTTOM].set3Points(nbl,nbr,fbr);
	pl[LEFT].set3Points(ntl,nbl,fbl);
	pl[RIGHT].set3Points(nbr,ntr,fbr);
	pl[NEARP].set3Points(ntl,ntr,nbr);
	pl[FARP].set3Points(ftr,ftl,fbl);
}


int FrustumG::pointInFrustum(glm::vec3 &p) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(p) < 0){
			/*switch (i){
			case 0: printf("TOP "); break;
			case 1:printf("BOTTOM "); break;
			case 2:printf("LEFT "); break;
			case 3:printf("RIGHT "); break;
			case 4:printf("NEARP "); break;
			case 5:printf("FARP "); break;
			}
			*/
			return OUTSIDE;
		}
	}
	return(result);

}


int FrustumG::sphereInFrustum(glm::vec3 &p, float raio) {

	int result = INSIDE;
	float distance;

	for(int i=0; i < 6; i++) {
		distance = pl[i].distance(p);
		if (distance < -raio)
			return OUTSIDE;
		else if (distance < raio)
			result =  INTERSECT;
	}
	return(result);

}


int FrustumG::boxInFrustum(AABox &b) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(b.getVertexP(pl[i].normal)) < 0)
			return OUTSIDE;
		else if (pl[i].distance(b.getVertexN(pl[i].normal)) < 0)
			result =  INTERSECT;
	}
	return(result);

 }



/*

void FrustumG::drawPoints() {


	glBegin(GL_POINTS);

		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);

		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	glEnd();
}


void FrustumG::drawLines() {

	glBegin(GL_LINE_LOOP);
	//near plane
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//far plane
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//bottom plane
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//top plane
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//left plane
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	// right plane
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	glEnd();
}


void FrustumG::drawPlanes() {

	glBegin(GL_QUADS);

	//near plane
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);

	//far plane
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	//bottom plane
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);

	//top plane
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);

	//left plane

		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);

	// right plane
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	glEnd();

}

void FrustumG::drawNormals() {

	glm::vec3 a,b;

	glBegin(GL_LINES);

		// near
		a = (ntr + ntl + nbr + nbl) * 0.25;
		b = a + pl[NEARP].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);

		// far
		a = (ftr + ftl + fbr + fbl) * 0.25;
		b = a + pl[FARP].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);

		// left
		a = (ftl + fbl + nbl + ntl) * 0.25;
		b = a + pl[LEFT].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);
		
		// right
		a = (ftr + nbr + fbr + ntr) * 0.25;
		b = a + pl[RIGHT].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);
		
		// top
		a = (ftr + ftl + ntr + ntl) * 0.25;
		b = a + pl[TOP].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);
		
		// bottom
		a = (fbr + fbl + nbr + nbl) * 0.25;
		b = a + pl[BOTTOM].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);

	glEnd();


}

*/