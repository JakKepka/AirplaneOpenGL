#version 330 core

layout (location = 0) in vec3 aPos;    // Pozycja wierzchołka
layout (location = 1) in vec2 aTexCoord; // Współrzędne tekstury

out vec2 TexCoord; // Współrzędne tekstury przekazywane do fragment shadera

uniform mat4 model; // Macierz modelu
uniform mat4 view;  // Macierz widoku
uniform mat4 projection; // Macierz projekcji

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord; // Przekazywanie współrzędnych tekstury
}
