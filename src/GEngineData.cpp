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

    return data;
}

}  // namespace galena
