#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 created_noise;

// Simple random function
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// Noise function
float noise(vec2 pos) {
    vec2 i = floor(pos);
    vec2 f = fract(pos);
    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main()
{
    vec4 worldPosition = Model * vec4(v_position, 1.0);
    gl_Position = Projection * View * worldPosition;

    // Generate noise based on texture coordinates
    float n = noise(v_texture_coord * 15.0);

    // Colors for brown and green
    vec3 brown = vec3(0.6, 0.3, 0.1);
    vec3 green = vec3(0.0, 0.5, 0.0);

    // Transition from brown to green using noise
    created_noise = mix(brown, green, n);
}

