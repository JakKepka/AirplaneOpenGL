#version 330 core

out vec4 FragColor;

in vec2 TexCoord; // Współrzędne tekstury otrzymane z vertex shadera

uniform sampler2D texture1; // Jednostka tekstury

void main()
{
    FragColor = texture(texture1, TexCoord); // Pobieranie koloru z tekstury
}
