#include "GEngineData.h"

#include "shaders/GShaderData.h"

namespace galena {

GEngineData GetEngineData()
{
    GEngineData data;

    std::vector<std::string> debugShaderUniforms = {"transform"};

    data.shaderData[GShaderKeyDebug] = GShaderData(
        GetCommon_vs(), GetDebug_fs(), std::move(debugShaderUniforms));

    std::vector<std::string> texturedQuadUniforms = {
        "transform", "sampleTransform", "sampleTexture"};

    data.shaderData[GShaderKeyTexturedQuad] = GShaderData(
        GetCommon_vs(), GetTexturedQuad_fs(), std::move(texturedQuadUniforms));

    return data;
}

}  // namespace galena
