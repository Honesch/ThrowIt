#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 M_Vl_Pl_Matrix;

void main() 
{
	gl_Position = M_Vl_Pl_Matrix * vec4(position, 1);
}