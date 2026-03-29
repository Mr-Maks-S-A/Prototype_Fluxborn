#version 450 core
out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 u_Color;

void main() {
    // Простейший направленный свет (как будто солнце)
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 norm = normalize(v_Normal);

    // Ambient (фоновый свет, чтобы тени не были черными)
    float ambient = 0.3;

    // Diffuse (основной свет)
    float diff = max(dot(norm, lightDir), 0.0);

    FragColor = u_Color * (ambient + diff);
}
