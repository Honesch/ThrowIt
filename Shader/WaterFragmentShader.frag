#version 330 core

uniform vec4 vBaseColorValue;
uniform vec3 ViewPos;
//uniform vec3 LightDirection;
 
uniform sampler2D waterReflection;
uniform sampler2D waterRefraction;
uniform sampler2D normalMap;
uniform sampler2D dudv;

in vec3 LightDirection;
in vec2 texturecoords;
in float time;
in vec4 worldPos;
 
layout(location = 0) out vec4 fragColor;
 
// Constants //
float kDistortion = 0.015;
float kReflection = 0.01;
vec4 baseColor = vBaseColorValue;
 
vec4 tangent = vec4(1.0, 0.0, 0.0, 0.0);
vec4 lightNormal = vec4(0.0, 1.0, 0.0, 0.0);
vec4 biTangent = vec4(0.0, 0.0, 1.0, 0.0);
 
void main(void)
{
    // Light Tangent Space //
    vec4 lightDirection = normalize(vec4(LightDirection.xyz, 1.0));
    vec4 lightTanSpace = normalize(vec4(dot(lightDirection, tangent), dot(lightDirection, biTangent), dot(lightDirection, lightNormal), 1.0));
 
    // Fresnel Term //
    vec4 distOffset = texture(dudv, texturecoords + vec2(time)) * kDistortion;
    vec4 normal = texture(normalMap, vec2(texturecoords + distOffset.xy));
    normal = normalize(normal * 2.0 - 1.0);
    normal.a = 0.81;
 
    vec4 lightReflection = normalize(reflect(-1 * lightTanSpace, normal));
    vec4 invertedFresnel = vec4(dot(normal, lightReflection));
    vec4 fresnelTerm = 1.0 - invertedFresnel;
   
//ViewPos = vec3(ViewPos.x + 20.0, ViewPos.y + 20.0, ViewPos.z + 20.0);
//vec4 fresnel = 0.02f+0.97f*pow((1-dot(vec4(ViewPos,1), normal)),5);
//fresnelTerm *= fresnel;
//invertedFresnel *= fresnelTerm +1;
//fresnelTerm = max(fresnelTerm * 0.1,0); //daempfung
//fresnelTerm = min(fresnelTerm,1); //daempfung


//vec4 invertedFresnel = fresnelTerm + 1;

    // Reflection //
    vec4 dudvColor = texture(dudv, vec2(texturecoords + distOffset.xy));
    dudvColor = normalize(dudvColor * 2.0 - 1.0) * kReflection;
 
    // Projection Coordinates from http://www.bonzaisoftware.com/tnp/gl-water-tutorial/
    vec4 tmp = vec4(1.0 / worldPos.w);
    vec4 projCoord = worldPos * tmp;
    projCoord += vec4(1.0);
    projCoord *= vec4(0.5);
    projCoord += dudvColor;
    projCoord = clamp(projCoord, 0.001, 0.999);
 
    vec4 reflectionColor = mix(texture(waterReflection, projCoord.xy), baseColor, 0.3);
    reflectionColor *= fresnelTerm;
 
    // Refraction //
    vec4 refractionColor = texture(waterRefraction, projCoord.xy);
    vec4 depthValue = vec4(0.1, 0.1, 0.1, 1.0);
    vec4 invDepth = 1.0 - depthValue;
 
    refractionColor *= invertedFresnel * invDepth;
    refractionColor += baseColor * depthValue * invertedFresnel;
 
    fragColor = reflectionColor + refractionColor;
//fragColor = texture(waterRefraction, texturecoords);
//fragColor = texture(normalMap, texturecoords);
	//fragColor = texture(waterReflection, projCoord.xy);
}