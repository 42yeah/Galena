#pragma once

#include <string>
#include <unordered_map>

namespace galena {

enum EGShaderKey
{
    GShaderKeyDebug = 0,
};

struct GShaderData
{
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

public:
    GShaderData() = default;

    GShaderData(std::string vertexSource, std::string fragmentSource)
        : vertexShaderSource(std::move(vertexSource)),
          fragmentShaderSource(std::move(fragmentSource))
    {
    }
};

struct GEngineData
{
    std::unordered_map<EGShaderKey, GShaderData> shaderData;
};

GEngineData GetEngineData();

}  // namespace galena
