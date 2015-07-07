#include "Kamera.h"
#include "Player.h"

const double Kamera::To_Radiants = M_PI / 180.0;

Kamera::Kamera(int fensterbreite, int fensterhoehe, GLFWwindow* window, glm::vec3 pos)
{
	init(pos);

	win = window;

	breite = fensterbreite;
	hoehe = fensterhoehe;

	mitteX = breite / 2.0f;
	mitteY = hoehe / 2.0f;

	glfwSetCursorPos(win, mitteX, mitteY);

	localViewdirection = calculateLocalViewdirection();
}


Kamera::~Kamera(void)
{
}

void Kamera::init(glm::vec3 pos)
{
	position = pos;

	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = 0.0f;

	geschwindigkeit = 3.0;
	yAcceleration = 0.0;

	vorwaerts = false;
	rueckwaerts = false;
	links = false;
	rechts = false;

	turnDownNumpad = false;
	turnLeftNumpad = false;
	turnRightNumpad = false;
	turnUpNumpad = false;

	onGround = false;
}

void Kamera::setPlayerSpeed(double speed)
{
	geschwindigkeit = speed;
}	

const double Kamera::inRadiant(const double &gradWinkel) const
{
	return gradWinkel * To_Radiants;
}

void Kamera::mousemove(int mouseX, int mouseY)
{
	
	double horizontal = (mouseX - mitteX ) * 0.2; //0.2 = Maussesibilitaet
	double vertikal = (mouseY - mitteY) * 0.2; //0.2 = Maussensibilitaet
	//printf("%.2f %.2f\n", horizontal, vertikal);

	rotation.x -= vertikal;
	rotation.y += horizontal;

	if(getXRot() < -80)
	{
		rotation.x = -80;
	}

	if(getXRot() > 80)
	{
		rotation.x = 80;
	}

	if(getYRot() < 0)
	{
		rotation.y += 360;
	}

	if(getYRot() > 360)
	{
		rotation.y += -360;
	}
}

void Kamera::teleport(glm::vec3 pos)
{
	position=position + glm::vec3(0,0,1);
}

void Kamera::move(double time, Modell* boden)
{
	glm::vec3 bewegung;
	bewegung.x = 0;
	bewegung.y = 0;
	bewegung.z = 0;

	double sinXRot = sin( inRadiant( rotation.x ) );
	double cosXRot = cos( inRadiant( rotation.x ) );
 
	double sinYRot = sin( inRadiant( rotation.y  ) );
	double cosYRot = cos( inRadiant( rotation.y  ) );
 
	double pitchLimitFactor = cosXRot; //um die z Achse starr zu halten falls man nach oben schaut beim gehen

	if (vorwaerts)
	{
		//bewegung.x += -sinYRot * pitchLimitFactor;
		bewegung.x += -sinYRot;
		bewegung.y += sinXRot;
		//bewegung.z += cosYRot * pitchLimitFactor;
		bewegung.z += cosYRot;

	}
 
	if (rueckwaerts)
	{
		//bewegung.x += sinYRot * pitchLimitFactor;
		bewegung.x += sinYRot;
		bewegung.y += -sinXRot;
		//bewegung.z += -cosYRot * pitchLimitFactor;
		bewegung.z += -cosYRot;
	}
 
	if (rechts)
	{
		bewegung.x += -cosYRot;
		bewegung.z += -sinYRot;
	}
 
	if (links)
	{
		bewegung.x += cosYRot;
		bewegung.z += sinYRot;
	}
	
	if(rechts||links||vorwaerts||rueckwaerts)
	{
		bewegung = glm::normalize(bewegung); 
		bewegung.y = 0;
		double framerateIndependentFactor = geschwindigkeit * time; //fuer framerateunabhaengigkeit	
		bewegung *= framerateIndependentFactor;
		position += bewegung;
		glm::vec3 localdir = calculateLocalViewdirection();
		//printf("%.2f / %.2f / %.2f || %.2f / %.2f / %.2f \n",localdir.x, localdir.y, localdir.z,getXPos(),getYPos(),getZPos());

	}
	

}

void Kamera::updateY(Modell* boden,  int player){
		float terrainY = boden->getYposAt(getXPos(),getZPos());

		if(abs((terrainY+ 0.4)-position.y)>0.02){
			if(terrainY + 0.4 > position.y){
				position.y = position.y + 0.1;
				onGround = true;
				yAcceleration = 0;
			}		
			if(terrainY + 0.4 < position.y){
				position.y = position.y - yAcceleration;
				yAcceleration += 0.008;
				onGround = false;
			}
		}
		//einfach den naehesten nehmen
		//position.y = terrainY *(-1) - 0.5;
}

void Kamera::resetkeys()
{
	vorwaerts = false;
	rueckwaerts = false;
	links = false;
	rechts = false;
}

glm::vec3 Kamera::calculateGlobalViewdirection(){
	return (position+calculateLocalViewdirection());
}

glm::vec3 Kamera::calculateLocalViewdirection(){
	return glm::normalize(glm::vec3(
		cos(inRadiant(rotation.x)) * -sin(inRadiant(rotation.y)),
		sin(inRadiant(rotation.x)),
		cos(inRadiant(rotation.x)) * cos(inRadiant(rotation.y))));
}

void Kamera::moveNumpad(double time_delta){
	if(turnDownNumpad) rotation.x -= 70 * time_delta;
	if(turnUpNumpad) rotation.x += 70 * time_delta;
	if(turnRightNumpad) rotation.y += 70 * time_delta;
	if(turnLeftNumpad) rotation.y -= 70 * time_delta;

	if(getXRot() < -80)
	{
		rotation.x = -80;
	}

	if(getXRot() > 80)
	{
		rotation.x = 80;
	}

	if(getYRot() < 0)
	{
		rotation.y += 360;
	}

	if(getYRot() > 360)
	{
		rotation.y += -360;
	}
}
