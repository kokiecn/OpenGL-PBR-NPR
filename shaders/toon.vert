#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

uniform Mat{
	vec4 aAmbient;
	vec4 aDiffuse;
	vec4 aSpeculer;
	float alpha;
};

out vec3 vWorldPosition;
out vec3 vWorldNormal;
out vec3 vWorldTangent;
out vec2 vUv;

out vec4 Ambient;
out vec4 Diffuse;
out vec4 Speculer;
out float Alpha;

out vec4 fragPosLight;

uniform mat4 Model;
uniform mat4 ModelIT;
uniform mat4 ViewProjection;

uniform mat4 lightProjection;

void main() {
  vec4 worldPosition = Model * position;
  gl_Position = ViewProjection * worldPosition;
  vWorldPosition = worldPosition.xyz / worldPosition.w;
  vWorldNormal = mat3(ModelIT) * normal;
  vUv = uv;
  vWorldTangent = mat3(ModelIT) * tangent;
  Ambient = aAmbient;
  Diffuse = aDiffuse;
  Speculer = aSpeculer;
  Alpha = alpha;
  // Calculates the position of the light fragment for the fragment shader
  fragPosLight = lightProjection * vec4(worldPosition);
}