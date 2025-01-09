#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;

uniform bool mixed;
uniform bool cycle;
uniform float elapsed_time;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec4 color;
    vec2 newCoord = texcoord;
    
    if (cycle) {
        newCoord.x += elapsed_time;
    }
    
    if (mixed) {
        vec4 color1 = texture(texture_1, newCoord);
        vec4 color2 = texture(texture_2, newCoord);
        color = mix(color1, color2, 0.49f); // ultimul parametru reprezinta factorul de interpolare intre cele 2 culori, avand valoare intre 0 si 1
    } else {
        color = texture(texture_1, newCoord);
    }
    
    if (color.a < 0.5f) {
        discard;
    }
    
    out_color = color;
}