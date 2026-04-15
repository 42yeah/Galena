#version 300 es

precision highp float;

layout (location = 0) in vec3 aPos;

out vec2 uv;

uniform mat4 transform;

void main()
{
    vec4 position = transform * vec4(aPos, 1.0);
    uv = vec2(aPos.xy * 0.5 + 0.5);
    uv.y = 1.0 - uv.y;
  
    gl_Position = position; 
}
