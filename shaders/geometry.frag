
#version 330 core
out vec4 FragColor;

uniform sampler2D texture;
in vec2 UV;


void main()
{
    FragColor = texture2D(texture,UV);
} 