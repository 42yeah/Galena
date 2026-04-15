#version 300 es

precision highp float;

in vec2 uv;

out vec4 color;

uniform sampler2D sampleTexture;
uniform int horizontal;

float Weights[5] = float[] (
    // Weights from: https://learnopengl.com/Advanced-Lighting/Bloom
    0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216    
);

void main()
{
    ivec2 texSize = textureSize(sampleTexture, 0);
    vec2 texOffset = vec2(1.0) / vec2(texSize);

    color = Weights[0] * texture(sampleTexture, uv);

    if (horizontal == 1)
    {
        for (int i = 1; i < 5; ++i)
        {
            float fi = float(i);
            color += Weights[i] * texture(sampleTexture, uv + vec2(texOffset.x * fi, 0.0));
            color += Weights[i] * texture(sampleTexture, uv - vec2(texOffset.x * fi, 0.0));
        }    
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            float fi = float(i);
            color += Weights[i] * texture(sampleTexture, uv + vec2(0.0, texOffset.y * fi));
            color += Weights[i] * texture(sampleTexture, uv - vec2(0.0, texOffset.y * fi));
        }
    }
}
