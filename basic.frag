#version 330 core

in vec4 channelCol;
in vec2 TexCoord;

out vec4 outCol;
uniform sampler2D uTexture;

void main()
{
        
        outCol = texture(uTexture, TexCoord);
    
}