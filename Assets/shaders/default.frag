#version 450 core
out vec4 FragColor;

uniform vec4 u_Color; // Добавляем входную переменную

void main() {
    FragColor = u_Color;
}
