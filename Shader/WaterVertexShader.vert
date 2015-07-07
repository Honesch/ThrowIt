#version 330 core


uniform mat4 mvpMatrix;
uniform float uniTime;
uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPOS;
 
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texCoords;
 
out vec2 texturecoords;
out float time;
out vec4 worldPos;
out vec3 LightDirection;
 
void main()
{
	gl_Position = mvpMatrix * vec4(pos,1);
 
	worldPos = mvpMatrix * vec4(pos,1);
 
	texturecoords = texCoords;
	time = uniTime / 20;

	vec3 vertexPosition_cameraspace = ( view * model * vec4(pos,1)).xyz;
	vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace = ( view * vec4(lightPOS,1)).xyz;
	LightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;
}