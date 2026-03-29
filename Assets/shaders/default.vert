#version 450 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 v_Normal;
out vec3 v_FragPos;

void main() {
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal; // Корректируем нормаль при вращении
    v_FragPos = vec3(u_Model * vec4(a_Pos, 1.0));
    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}
