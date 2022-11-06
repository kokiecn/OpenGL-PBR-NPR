#version 460

in vec3 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec2 vUv;

in vec4 Ambient;
in vec4 Diffuse;
in vec4 Speculer;
in float Alpha;

in vec4 fragPosLight;
layout (location = 0) out vec4 fragment;

const float PI = 3.1415;
const float lightIntensity =800;

uniform vec3 worldCameraPosition;

layout (binding = 0) uniform sampler2D albedoMap;

uniform bool albedoFlag;
uniform bool normFlag;
uniform bool roughFlag;
uniform bool metalFlag;

uniform vec3 lightPos1;
uniform vec3 lightPosPoint;

uniform vec4 lightColor;
uniform sampler2D shadowMap;

const float RimAmount = 0.9;

vec4 point(){
	vec4 albedo = albedoFlag ? texture(albedoMap, vUv) : Diffuse;
	vec3 normal = normalize(vWorldNormal);
	vec3 lightVec = lightPosPoint - vWorldPosition;
	vec3 lightDirection = normalize(lightVec);

	float NoL = max(dot(normal, lightDirection), 0);

	vec3 viewDirection = normalize(worldCameraPosition - vWorldPosition);
	vec3 halfwayVec = normalize(viewDirection + lightDirection);

	float specIntensity = pow(max(dot(normal, halfwayVec), 0.0f), 32 * 32);
	float specularIntensitySmooth = smoothstep(0.005f, 0.01f, specIntensity);
	
	float lightIntensity =smoothstep(0, 0.01f,NoL);

	float rimDot = 1 - dot(viewDirection, normal);
	float rimIntensity = rimDot * NoL;
	float rimIntensitySmooth = smoothstep(RimAmount - 0.01,RimAmount + 0.01, rimDot);
	
	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 0.03;
	float b = 0.05;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f) / (4.0f * PI) * 10;


	return albedo  * (0.3f + lightIntensity * inten + specularIntensitySmooth  * inten);// + rimIntensitySmooth);
}


vec4 directional(){

	vec4 albedo = albedoFlag ? texture(albedoMap, vUv) : Diffuse;
	vec3 normal = normalize(vWorldNormal);
	vec3 lightVec = lightPos1 - vWorldPosition;

	vec3 lightDirection = normalize(lightVec);

	float NoL = max(dot(normal, lightDirection), 0);

	vec3 viewDirection = normalize(worldCameraPosition - vWorldPosition);
	vec3 halfwayVec = normalize(viewDirection + lightDirection);

	float specIntensity = pow(max(dot(normal, halfwayVec), 0.0f), 32 * 32);
	float specularIntensitySmooth = smoothstep(0.005f, 0.01f, specIntensity);
	
	float lightIntensity =smoothstep(0, 0.1f,NoL);

	float rimDot = 1 - dot(viewDirection, normal);
	float rimIntensity = rimDot * NoL;
	float rimIntensitySmooth = smoothstep(RimAmount - 0.01,RimAmount + 0.01, rimDot);
	

	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.005f * (1.0f - dot(normal, lightDirection)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);

	}
	return  albedo* ( 1 - shadow) * (lightIntensity + specularIntensitySmooth * (1.0f - shadow)  + rimIntensitySmooth) +0.1f;
}


void main() 
{

	fragment = directional();
	fragment += point();

}