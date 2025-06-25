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
    
    // Create 3D sphere appearance
    float z = sqrt(1.0 - dist * dist);
    vec3 normal = normalize(vec3(coord, z));
    
    // Simple lighting
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.5));
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.3;
    float lighting = ambient + diffuse * 0.7;
    
    // Soft edges for merging
    float alpha = 1.0 - smoothstep(0.7, 1.0, dist);
    
    // Enhanced color rendering with glow effect
    vec3 finalColor = FragColor * lighting;
    
    // Add slight glow to center
    float glow = 1.0 - dist;
    finalColor += FragColor * glow * 0.3;
    
    // Ensure minimum brightness
    finalColor = max(finalColor, FragColor * 0.2);
    
    FragColorOut = vec4(finalColor, alpha * 0.9);
}