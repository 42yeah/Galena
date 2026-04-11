#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace galena {

enum EGShaderKey
{
    GShaderKeyDebug = 0,
    GShaderKeyTexturedQuad,
    GShaderKeyInvert,
    GShaderKeyGaussianBlur,
    GShaderKeyAdd
};

struct GShaderData
{
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    std::vector<std::string> uniforms;

public:
    GShaderData() = default;

    GShaderData(std::string vertexSource, std::string fragmentSource,
        std::vector<std::string> uniforms)
        : vertexShaderSource(std::move(vertexSource)),
          fragmentShaderSource(std::move(fragmentSource)),
          uniforms(std::move(uniforms))
    {
    }
};

struct GEngineData
{
    std::unordered_map<EGShaderKey, GShaderData> shaderData;
};

GEngineData GetEngineData();

}  // namespace galena
