#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec2 texCoord;

flat out int flaeche;
smooth out vec3 vNormal;

void main()
{
	gl_Position = projectionMatrix*modelViewMatrix*normalMatrix*vec4(inPosition, 1.0);
	texCoord = inCoord;
	vec4 vRes = normalMatrix*vec4(inNormal, 0.0);
	vNormal = vRes.xyz;
	
	flaeche = 0;
	if(inNormal == vec3(0.0,0.0,-1.0)) flaeche = 1;
	if(inNormal == vec3(0.0,0.0,1.0)) flaeche = 2;
	if(inNormal == vec3(1.0,0.0,0.0)) flaeche = 3;
	if(inNormal == vec3(-1.0,0.0,0.0)) flaeche = 4;
	if(inNormal == vec3(0.0,-1.0,0.0)) flaeche = 5;
	if(inNormal == vec3(0.0,1.0,0.0)) flaeche = 6;
}