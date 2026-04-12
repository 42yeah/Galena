#include "GEngineData.h"

#include "shaders/GShaderData.h"

namespace galena {

GEngineData GetEngineData()
{
    GEngineData data;

    std::vector<std::string> debugUniforms = {"transform"};

    data.shaderData[GShaderKeyDebug] =
        GShaderData(GetCommon_vs(), GetDebug_fs(), std::move(debugUniforms));

    std::vector<std::string> texturedQuadUniforms = {
        "transform", "sampleTransform", "sampleTexture"};

    data.shaderData[GShaderKeyTexturedQuad] = GShaderData(
        GetCommon_vs(), GetTexturedQuad_fs(), std::move(texturedQuadUniforms));

    std::vector<std::string> invertUniforms = {"sampleTexture"};

    data.shaderData[GShaderKeyInvert] = GShaderData(
        GetCommonFramebuffer_vs(), GetInvert_fs(), std::move(invertUniforms));

    std::vector<std::string> bloomUniforms = {"sampleTexture", "horizontal"};

    data.shaderData[GShaderKeyGaussianBlur] =
        GShaderData(GetCommonFramebuffer_vs(), GetGaussianBlur_fs(),
            std::move(bloomUniforms));

    std::vector<std::string> blendUniforms = {"textureA", "textureB"};

    data.shaderData[GShaderKeyBlend] = GShaderData(
        GetCommonFramebuffer_vs(), GetBlend_fs(), std::move(blendUniforms));

    std::vector<std::string> crtUniforms = {
        "sampleTexture", "framebufferResolution"};

    data.shaderData[GShaderKeyCRT] = GShaderData(
        GetCommonFramebuffer_vs(), GetCRT_fs(), std::move(crtUniforms));

    return data;
}

}  // namespace galena
