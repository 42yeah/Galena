#version 300 es

precision highp float;

in vec2 uv;

out vec4 color;

uniform sampler2D sampleTexture;
uniform vec2 framebufferResolution;

const float Pi = 3.14159265;

const float SharpnessH = 0.5;
const float SharpnessV = 1.0;

const float GammaInput = 2.0;
const float GammaOutput = 1.8;
const float Dilation = 1.0;

float CurveDistance(float x, float sharp)
{
    float xStep = step(0.5, x);
    float curve = 0.5 - sqrt(0.25 - (x - xStep) * (x - xStep)) * sign(0.5 - x);

    return mix(x, curve, sharp);
}

void main()
{
    /* Stolen shamelessly from the awesome crt-easymode:
       https://github.com/libretro/common-shaders/blob/master/crt/shaders/crt-easymode.cg
    */

    vec2 texSize = vec2(textureSize(sampleTexture, 0));

    vec2 dx = vec2(1.0 / texSize.x, 0.0);
    vec2 dy = vec2(0.0, 1.0 / texSize.y);

    vec2 pixCoord = uv * texSize - vec2(0.5, 0.5);
    vec2 texCoord = (floor(pixCoord) + vec2(0.5, 0.5)) / texSize;

    vec2 dist = fract(pixCoord);

    float curveX = CurveDistance(dist.x, SharpnessH);

    vec3 colorH = mix(texture(sampleTexture, texCoord).rgb,
        texture(sampleTexture, texCoord + dx).rgb, curveX);

    vec3 colorV = mix(texture(sampleTexture, texCoord + dy).rgb,
        texture(sampleTexture, texCoord + dx + dy).rgb, curveX);

    vec3 smearColor = mix(colorH, colorV, CurveDistance(dist.y, SharpnessV));

    smearColor = pow(smearColor, vec3(GammaInput / (1.0 + Dilation)));

    float luma = dot(vec3(0.2126, 0.7152, 0.0722), smearColor);

    float brightness =
        0.5 * (max(smearColor.r, max(smearColor.g, smearColor.b)) + luma);

    const float ScanlineBrightnessMin = 0.35;
    const float ScanlineBrightnessMax = 0.65;
    const float ScanlineBeamWidthMin = 1.5;
    const float ScanlineBeamWidthMax = 1.5;
    const float ScanlineStrength = 1.0;

    float scanBrightness =
        clamp(brightness, ScanlineBrightnessMin, ScanlineBrightnessMax);

    float scanBeam = clamp(scanBrightness * ScanlineBeamWidthMax,
        ScanlineBeamWidthMin, ScanlineBeamWidthMax);

    float scanWeight =
        1.0 - pow(cos(uv.y * 2.0 * Pi * texSize.y) * 0.5 + 0.5, scanBeam) *
                  ScanlineStrength;

    const float MaskStrength = 0.3;
    const float MaskSize = 1.0;
    const float MaskDotWidth = 1.0;
    const float MaskDotHeight = 1.0;
    const float MaskStagger = 0.0;

    float mask = 1.0 - MaskStrength;

    vec2 modFactor =
        uv * framebufferResolution * vec2(MaskSize, MaskSize * MaskDotHeight);

    int dotIdx = int(
        mod((modFactor.x + mod(modFactor.y, 2.0) * MaskStagger) / MaskDotWidth,
            3.0));

    vec3 maskWeight = vec3(0.0);

    if (dotIdx == 0)
    {
        maskWeight = vec3(1.0, mask, mask);
    }
    else if (dotIdx == 1)
    {
        maskWeight = vec3(mask, 1.0, mask);
    }
    else if (dotIdx == 2)
    {
        maskWeight = vec3(mask, mask, 1.0);
    }

    vec3 currColor = smearColor;
    vec3 outColor = currColor;

    outColor *= scanWeight;
    outColor = mix(outColor, currColor, scanBrightness);
    outColor *= maskWeight;
    outColor = pow(outColor, vec3(1.0 / GammaOutput));

    const float BrightnessModifier = 1.2;

    color = vec4(BrightnessModifier * outColor, 1.0);
}
