#version 330 core

in vec2 TexCoord;

uniform sampler2D ourTexture;
out vec4 FragColor; 

void main() {
    vec4 texColor = texture(ourTexture, TexCoord); 
    FragColor = texColor;

}

