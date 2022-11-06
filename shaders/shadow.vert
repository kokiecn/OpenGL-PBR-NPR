#version 460
layout (location = 0) in vec3 aPos;

uniform Mat{
	vec4 aAmbient;
	vec4 aDiffuse;
	vec4 aSpeculer;
	float alpha;
};

uniform mat4 lightProjection;
uniform mat4 model;

out float Alpha;

void main()
{
    gl_Position = lightProjection * model * vec4(aPos, 1.0);
	Alpha = alpha;
}