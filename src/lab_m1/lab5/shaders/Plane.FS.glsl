#version 330

//input
in vec3 created_noise;

layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(abs(created_noise), 1);
}