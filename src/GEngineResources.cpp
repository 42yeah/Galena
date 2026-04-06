#include "GEngineResources.h"
#include "GEngineData.h"
#include "GHwBuffer.h"
#include "GShader.h"

#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include <GLES3/gl3.h>

namespace galena {

GEngineResources::~GEngineResources() {}

std::unique_ptr<GEngineResources> GEngineResources::Create()
{
    GEngineData data = GetEngineData();

    std::unordered_map<EGShaderKey, std::unique_ptr<GShader>> shaders;

    for (std::pair<EGShaderKey, const GShaderData &> it : data.shaderData)
    {
        std::unique_ptr<GShader> shader =
            GShader::Create(it.second.vertexShaderSource,
                it.second.fragmentShaderSource, it.second.uniforms);

        if (!shader)
            return nullptr;

        shaders[it.first] = std::move(shader);
    }

    const float triangleData[] = {
        // clang-format off
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
        // clang-format on
    };

    GVertexBufferAttribs attrs(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    std::vector<GVertexBufferAttribs> attribs = {attrs};

    std::unique_ptr<GHwBuffer> triangleBuffer =
        GHwBuffer::Create(triangleData, sizeof(triangleData), attribs);

    const float quadData[] = {
        // clang-format off
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f
        // clang-format on
    };

    std::unique_ptr<GHwBuffer> quadBuffer =
        GHwBuffer::Create(quadData, sizeof(quadData), attribs);

    return std::make_unique<GEngineResources>(std::move(data),
        std::move(shaders), std::move(triangleBuffer), std::move(quadBuffer));
}

}  // namespace galena
