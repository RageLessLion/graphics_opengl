#version 330 core

// Input vertex position
layout(location = 0) in vec3 in_position;

// Output color to the Fragment Shader
out vec3 vertexColor;

// Uniforms for transformations
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Uniform colors for interpolation
uniform vec3 color1;  // Example: low elevation color
uniform vec3 color2;  // Example: high elevation color

// Frequency for noise function
uniform float frequency;

// Simple noise function (replace with a better one if needed)
float noise(vec2 pos) {
    return fract(sin(dot(pos, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    // Compute noise value using position.xz and frequency
    float noiseValue = noise(in_position.xz * frequency);

    // Adjust the Y-coordinate (height) using the noise value
    vec3 position = in_position;
    position.y += noiseValue;

    // Interpolate between color1 and color2
    vertexColor = mix(color1, color2, noiseValue);

    // Compute final position
    gl_Position = projection * view * model * vec4(position, 1.0);
}
