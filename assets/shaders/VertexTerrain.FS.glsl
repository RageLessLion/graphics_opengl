#version 330 core

// Input color from the Vertex Shader
in vec3 vertexColor;

// Output color
out vec4 fragColor;

void main()
{
    fragColor = vec4(vertexColor, 1.0);
}
