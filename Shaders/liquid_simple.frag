#version 450 core

in vec3 FragColor;
in float Radius;

out vec4 FragColorOut;

void main() {
    // Simple circle test
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = length(coord);
    
    if (dist > 1.0) {
        discard;
    }
    
    // Just output the color directly
    FragColorOut = vec4(FragColor, 1.0);
}