#version 300 es

precision highp float;

in vec2 uv;

out vec4 color;

uniform mat4 sampleTransform;
uniform sampler2D sampleTexture;

void main()
{
    vec2 samplePos = vec2(sampleTransform * vec4(uv, 1.0, 1.0));
    color = texture(sampleTexture, samplePos);
}
