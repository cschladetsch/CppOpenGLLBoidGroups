#version 450 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;

void main() {
    vec3 wallColor = vec3(0.95, 0.95, 0.98);
    vec3 lightDir1 = normalize(vec3(0.5, 0.8, 0.3));
    vec3 lightDir2 = normalize(vec3(-0.3, 0.9, -0.4));
    
    vec3 norm = normalize(Normal);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    float diff2 = max(dot(norm, lightDir2), 0.0) * 0.3;
    
    vec3 diffuse = vec3(0.9, 0.9, 0.95) * (diff1 + diff2);
    vec3 ambient = vec3(0.4, 0.4, 0.45);
    
    vec3 result = ambient + diffuse * wallColor;
    FragColor = vec4(result, 1.0);
}