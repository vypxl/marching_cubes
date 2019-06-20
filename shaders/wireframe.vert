#version 300 es

precision mediump float;

in vec3 position;
out vec3 _pos;

uniform mat4 MVP;

void main() {
    _pos = position;
    gl_Position = MVP * vec4(position, 1.0);
}
