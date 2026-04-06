#include "GEngineData.h"

#include "shaders/GShaderData.h"

namespace galena {

GEngineData GetEngineData()
{
    GEngineData data;

    GShaderData shaderData = GShaderData(GetCommon_vs(), GetDebug_fs());
    data.shaderData[GShaderKeyDebug] = shaderData;

    return data;
}

}  // namespace galena
