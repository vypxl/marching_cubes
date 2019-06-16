#version 300 es

precision mediump float;

in vec3 position;
out vec3 _pos;

uniform mat4 MVP;
uniform float surface;

void main() {
    _pos = position;
}
