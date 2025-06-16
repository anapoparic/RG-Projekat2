#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoord;
uniform vec4 uColor;

uniform mat4 uM; //Matrica transformacije
uniform mat4 uV; //Matrica kamere
uniform mat4 uP; //Matrica projekcija

out vec4 channelCol;
out vec2 TexCoord;

void main()
{
	gl_Position = uP * uV * uM * vec4(inPos, 1.0);
	channelCol = uColor;
	TexCoord = inTexCoord;
}