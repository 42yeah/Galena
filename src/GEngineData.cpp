#include "GEngineData.h"

#include "shaders/GShaderData.h"

namespace galena {

GEngineData GetEngineData()
{
    GEngineData data;

    std::vector<std::string> debugShaderUniforms = {"transform"};

    GShaderData debugShaderData = GShaderData(
        GetCommon_vs(), GetDebug_fs(), std::move(debugShaderUniforms));

    data.shaderData[GShaderKeyDebug] = std::move(debugShaderData);

    return data;
}

}  // namespace galena
