#version 330

in vec2 texCoord;
flat in int flaeche;
smooth in vec3 vNormal;
out vec4 fragColor;

uniform sampler2D gSampler1;
uniform sampler2D gSampler2;
uniform sampler2D gSampler3;
uniform sampler2D gSampler4;
uniform sampler2D gSampler5;
uniform sampler2D gSampler6; 

uniform vec4 vColor;

struct SimpleDirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbientIntensity;
};

uniform SimpleDirectionalLight sunLight;

void main()
{
	vec4 vTexColor = vec4(1.0,1.0,1.0,1.0);
	if(flaeche == 1) vTexColor = texture2D(gSampler1, texCoord);
	if(flaeche == 2) vTexColor = texture2D(gSampler2, texCoord);
	if(flaeche == 3) vTexColor = texture2D(gSampler3, texCoord);
	if(flaeche == 4) vTexColor = texture2D(gSampler4, texCoord);
	if(flaeche == 5) vTexColor = texture2D(gSampler5, texCoord);
	if(flaeche == 6) vTexColor = texture2D(gSampler6, texCoord);

	float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -sunLight.vDirection));
	fragColor = vTexColor*vColor*vec4(sunLight.vColor*(sunLight.fAmbientIntensity+fDiffuseIntensity), 1.0);
}