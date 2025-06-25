#version 450 core

in vec3 FragColor;
in float Radius;

out vec4 FragColorOut;

void main() {
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = length(coord);
    
    if (dist > 1.0) {
        discard;
    }
    
    float alpha = 1.0 - smoothstep(0.7, 1.0, dist);
    float centerIntensity = 1.0 - dist * 0.3;
    
    FragColorOut = vec4(FragColor * centerIntensity, alpha * 0.8);
}