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

uniform vec3 worldCameraPosition;
const float PI = 3.1415;

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D roughnessMap;
layout (binding = 3) uniform sampler2D metalnessMap;


uniform bool albedoFlag;
uniform bool normFlag;
uniform bool roughFlag;
uniform bool metalFlag;

uniform vec3 lightPos1;
uniform vec3 lightPosPoint;
uniform vec4 lightColor;
uniform sampler2D shadowMap;

const float lightIntensity =800;

vec3 GetNormal() {
  vec3 normal = normalize(vWorldNormal);
  vec3 bitangent = normalize(cross(normal, normalize(vWorldTangent)));
  vec3 tangent = normalize(cross(bitangent, normal));
  mat3 TBN = mat3(tangent, bitangent, normal);
  vec3 normalFromMap = texture(normalMap, vUv).rgb * 2 - 1;
  return normalize(TBN * normalFromMap);
}

vec3 NormalizedLambert(vec3 diffuseColor) {
  return diffuseColor / PI;
}

vec3 F_Schlick(vec3 F0, vec3 H, vec3 V) {
  return (F0 + (1.0 - F0) * pow(1.0 - max(dot(V, H), 0), 5.0));
}

float D_GGX(float a, float NoH) {
  float a2 = a * a;
  float NoH2 = NoH * NoH;
  float d = NoH2 * (a2 - 1.0) + 1.0;
  return a2 / (PI * d * d);
}

float G_SchlickGGX(float a, float NoV) {
  float EPSILON = 0.001;
  float k = a * a / 2 + EPSILON;
  float up = NoV;
  float down = NoV * (1 - k) + k;
  return up / down;
}

float G_Smith(float a, float NoV, float NoL) {
  float ggx0 = G_SchlickGGX(1, NoV);
  float ggx1 = G_SchlickGGX(1, NoL);
  return ggx0 * ggx1;
}


float shadow(vec3 normal, vec3 lightDirection){
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
		float bias = max(0.001f * (1.0f - dot(normal, lightDirection)), 0.0005f);

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
	return shadow;
}


vec3 BRDF(vec3 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 L, vec3 H, bool shading) {
  float EPSILON = 0.001;
  vec3 F0 = mix(vec3(0.04), albedo, metallic);
  float NoH = max(dot(N, H), 0);
  float NoV = max(dot(N, V), 0);
  float NoL = max(dot(N, L), 0);
  float a = roughness * roughness;

  // specular
  vec3 F = F_Schlick(F0, H, V);
  float D = D_GGX(a, NoH);
  float G = G_Smith(a, NoV, NoL);
  vec3 up = F * D * G;
  float down = max(4.0 * NoV * NoL, EPSILON);
  vec3 specular = up / down;

  // diffuse
  vec3 diffuse = NormalizedLambert(albedo);

  vec3 kD = vec3(1.0) - F;
  kD *= 1.0 - metallic;
  float shadow = shading ? shadow(N,L):0.0f;

  return kD * diffuse * (1 - shadow + 0.1f) + specular * (1 - shadow);
}


void main() 
{
	
	vec4 albedo = albedoFlag ? texture(albedoMap, vUv) : Diffuse;
	float metal =  metalFlag ? texture(metalnessMap, vUv).r : 0;
	float roughness = roughFlag ? texture(roughnessMap, vUv).r :1-Speculer.r;

	vec3 lightVec = lightPos1 -vWorldPosition;

	//lightVec = vec3(0.0f,1.0f,1.0f);
	vec3 normal = GetNormal(); 
	vec3 lightDirection = normalize(lightVec);
	vec3 viewDirection = normalize(worldCameraPosition - vWorldPosition);
	vec3 halfwayVec = normalize(viewDirection + lightDirection);

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 0.1;
	float b = 0.5;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f) * lightIntensity / (4.0f * PI);

	vec3 c = BRDF(albedo.rgb, metal, roughness, normal, viewDirection, lightDirection, halfwayVec, true) * 10;
	
	lightVec = lightPosPoint -vWorldPosition;
	lightDirection = normalize(lightVec);
	 halfwayVec = normalize(viewDirection + lightDirection);

	c += BRDF(albedo.rgb, metal, roughness, normal, viewDirection, lightDirection, halfwayVec,false) * inten * 7;

	fragment = vec4(c, Alpha);
}