#version 300 es

precision highp float;

in vec2 uv;

out vec4 color;

uniform sampler2D textureA;
uniform sampler2D textureB;

// Simply add these two colors together

void main()
{
    vec4 colorA = texture(textureA, uv);
    vec4 colorB = texture(textureB, uv);

    color = vec4(mix(colorA, colorB, 1.0 - colorA.w).xyz, 1.0);
}
