#version 450 core

layout(location = 0) out vec4 color;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D u_Texture;
uniform vec4 u_Color;
uniform float u_LightIntensity; // Новое!
uniform vec3 u_LightDir;        // Новое!

void main() {
    vec4 texColor = texture(u_Texture, TexCoords);
    if(texColor.a < 0.1) discard;

    // Расчет освещения (Lambert)
    // max(..., 0.2) — это минимальный фоновый свет (Ambient), чтобы тени не были черными
    float diff = max(dot(normalize(Normal), u_LightDir), 0.2);

    // Применяем интенсивность к освещению
    float finalLight = diff * u_LightIntensity;

    color = texColor * u_Color * finalLight;
}
