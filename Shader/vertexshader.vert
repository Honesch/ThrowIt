#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 uv;

out vec3 normal;
out vec2 texCoords;
out vec4 smCoords;//shadow mapping coords
//fuer beleuchtung
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPOS;

uniform mat4 M_Vl_Pl_L_Matrix; //fuer shadow mapping
uniform vec4 u_plane0=vec4(0.0,1.0,0.0,0.0); // in viewspace, fuers clippen

void main() 
{

	gl_Position = view * model * vec4(position,1);
	normal = (model * vec4(Normal,0)).xyz;
	texCoords = uv;

	smCoords = M_Vl_Pl_L_Matrix * vec4(position, 1);//berechne sm koordinaten

	//fuer beleuchtung
	Position_worldspace = (model * vec4(position,1)).xyz;

	Normal_cameraspace = ( view * model * vec4(normal,0)).xyz;

	vec3 vertexPosition_cameraspace = ( view * model * vec4(position,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace = ( view * vec4(lightPOS,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	vec4 posviewspace = /*view * */  model * vec4(position, 1);//vielleicht nur model ohne view
	gl_ClipDistance[0] = dot(u_plane0,posviewspace);

}