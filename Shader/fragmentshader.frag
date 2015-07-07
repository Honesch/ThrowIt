#version 330 core

in vec3 normal;
in vec2 texCoords;
in vec4 smCoords;

//fuer beleuchtung
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

uniform sampler2D color_texture;
uniform sampler2D shadowMap;

//fuer beleuchtung
uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPOS;
uniform vec3 light2POS;

uniform vec3 diffuse;
uniform vec3 specular;


layout(location = 0) out vec4 fragColor;

void main()
{
	float alpha = 0.2; //vielleicht als Uniform

	vec2 sm_uvs = smCoords.xy / smCoords.w;
	float depth_p = smCoords.z / smCoords.w;
	float depth_c = texture(shadowMap, sm_uvs).r;
	//float depth_c = textureProj(shadowMap, smCoords).r;

float shadowMult = (depth_c < depth_p) ? 0.0 : 1.0;
	//float shadowMult = textureProj(shadowMap, smCoords).r;	

	//fragColor = vec4((normal+1)*0.5, 1.0);//nur normalen
	//fragColor = shadowMult * vec4(texture(color_texture, texCoords).rgb, 1.0); //ohne beleuchtung aber schatten
	//fragColor = vec4(texture(color_texture, texCoords).rgb, 1.0);	//ohne schatten, ohne beleuchtung

	//beleuchtung
	vec3 ambient = texture(color_texture, texCoords).rgb;
	
	vec3 LightColor = vec3(1,1,1); //vielleicht als uniform
	float LightPower = 50.0f;	//vielleicht als uniform
			
	float LightDistance = length(lightPOS - Position_worldspace );
	vec3 n = normalize( Normal_cameraspace ); //normalen auf laenge 1 normalisieren
	vec3 l = normalize( LightDirection_cameraspace ); //richtung vom Fragment zum Licht
	float cosTheta = clamp( dot( n,l ), 0,1 ); //clamp, damit der winkel nicht negativ werden kann

	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l,n); //wo wird hin reflektiert

	float cosAlpha = clamp( dot( E,R ), 0,1 );

	fragColor =(vec4(ambient,alpha)*0.0001)+ (shadowMult * vec4(ambient+ diffuse * LightColor * LightPower * cosTheta / (LightDistance*LightDistance) 
		 + specular * LightColor * LightPower * pow(cosAlpha,5) / (LightDistance*LightDistance), alpha));

}