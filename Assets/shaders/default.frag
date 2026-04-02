#version 450 core

layout(location = 0) out vec4 color;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D u_Texture;
uniform vec4 u_Color;
uniform float u_LightIntensity;
uniform vec3 u_LightDir;
uniform int u_DebugMode; // 0 - Normal, 1 - Visualize Normals, 2 - White Mesh

void main() {
    if (u_DebugMode == 1) {
        // Каждое направление стенки будет иметь свой цвет
        color = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
        return;
    }

    vec4 texColor = texture(u_Texture, TexCoords);
    if(texColor.a < 0.1) discard;

    if (u_DebugMode == 2) {
        color = vec4(1.0, 1.0, 1.0, 1.0); // Просто белый меш для проверки геометрии
        return;
    }

    float diff = max(dot(normalize(Normal), normalize(u_LightDir)), 0.2);
    float finalLight = diff * u_LightIntensity;

    color = texColor * u_Color * finalLight;
}
