#version 300 es

precision highp float;

in vec2 uv;

out vec4 color;

uniform sampler2D textureA;
uniform sampler2D textureB;

// Simply add these two colors together

void main()
{
    vec3 colorA = texture(textureA, uv).rgb;
    vec3 colorB = texture(textureB, uv).rgb;

    color = vec4(min(vec3(1.0), colorA + colorB), 1.0);
}
