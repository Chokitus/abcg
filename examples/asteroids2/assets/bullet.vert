#version 410

layout(location = 0) in vec2 inPosition;

uniform vec4 color;
uniform float rotation;
uniform float scale;
uniform vec2 translation;
uniform float pointSize;

out vec4 fragColor;

void main() {
  float sinAngle = sin(rotation);
  float cosAngle = cos(rotation);
  vec2 rotated = vec2(inPosition.x * cosAngle - inPosition.y * sinAngle,
                      inPosition.x * sinAngle + inPosition.y * cosAngle);

  vec2 newPosition = rotated * scale + translation;
  gl_Position = vec4(newPosition, 0, 1);
  gl_PointSize = pointSize;
  fragColor = color;
}