#version 300 es

precision mediump float;

in vec3 pos;

out vec4 color;

float lerp(float val) {
  return val / 10.0;
}

void main() {
  color = vec4(
    lerp(pos.x),
    lerp(pos.y),
    lerp(pos.z),
    1.0
  );
}
