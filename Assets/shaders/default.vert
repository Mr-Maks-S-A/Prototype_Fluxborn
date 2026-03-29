#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main() {
    TexCoords = aTexCoords;
    // Передаем нормаль, учитывая вращение объекта (если оно есть)
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
}
