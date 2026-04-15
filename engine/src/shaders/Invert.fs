#version 300 es

precision highp float;

in vec2 uv;

out vec4 color;

uniform sampler2D sampleTexture;

void main()
{
    color = texture(sampleTexture, uv);
    color.rgb = 1.0 - color.rgb;
}
