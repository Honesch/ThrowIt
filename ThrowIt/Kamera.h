#pragma once
#include "Modell.h"
#include <glfw3.h>
#include <glm\glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h> 



class Kamera
{
	private:
		GLFWwindow* win;

		glm::vec3 position;

		double geschwindigkeit;

		int hoehe;
		int breite;
		int mitteX;
		int mitteY;

		void init(glm::vec3 pos);

	public:
		Kamera(int fensterbreite, int fensterhoehe, GLFWwindow* window, glm::vec3 pos);
		~Kamera(void);

		static const double To_Radiants;
		void setPlayerSpeed(double speed);

		bool vorwaerts;
		bool rueckwaerts;
		bool links;
		bool rechts;
		bool turnLeftNumpad;
		bool turnRightNumpad;
		bool turnUpNumpad;
		bool turnDownNumpad;
		float yAcceleration;
		bool onGround;
		

		void resetkeys();
		void mousemove(int mouseX, int mouseY);
		const double inRadiant(const double &gradWinkel) const;
		void move(double time, Modell* boden);
		void moveNumpad(double time_delta);
		void teleport(glm::vec3 pos);
		void updateY(Modell* boden, int player);
		glm::vec3 calculateGlobalViewdirection();
		glm::vec3 calculateLocalViewdirection();

		glm::vec3 globalViewdirection;
		glm::vec3 localViewdirection;
		glm::vec3 rotation;
		glm::mat4 mat;

		glm::vec3 getPosition() const {return position;}
		double getXPos()		const {return position.x;}
		double getYPos()		const {return position.y;}
		double getZPos()		const {return position.z;}

		glm::vec3 getRotation() const { return rotation; }
		double getXRot()           const { return rotation.x; }
		double getYRot()           const { return rotation.y; }
		double getZRot()           const { return rotation.z; }
	  
};

