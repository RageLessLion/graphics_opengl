#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform bool billboard;

// Output
out vec2 texcoord;


void main()
{
    texcoord = v_texture_coord;
    mat4 MVP = Projection * View * Model;
    
    vec3 pos = (billboard) ? inverse(mat3(View)) * v_position : v_position;
    gl_Position = MVP * vec4(pos, 1.0);
}