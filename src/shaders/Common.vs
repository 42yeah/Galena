#version 300 es

precision highp float;

layout (location = 0) in vec3 aPos;

uniform mat4 transform;

out vec2 uv;

void main()
{
  vec4 position = transform * vec4(aPos, 1.0);
  uv = vec2(position.xy);
  
  gl_Position = position; 
}
