#include "GEngineResources.h"
#include "GEngineData.h"
#include "GHwBuffer.h"
#include "GShader.h"

#include "GTexture.h"
#include "Galena/GEngineDesc.h"
#include "Galena/GTextureDesc.h"

#include <memory>
#include <unordered_map>

#include <GLES3/gl3.h>

namespace galena {

GEngineResources::~GEngineResources() {}

std::unique_ptr<GEngineResources> GEngineResources::Create(
    const GEngineDesc &desc)
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

    std::unordered_map<uint32_t, std::unique_ptr<GTexture>> textures;

    for (const std::pair<uint32_t, const GTextureDesc &> entry : desc.textures)
    {
        const uint32_t textureKey = entry.first;
        const GTextureDesc &textureDesc = entry.second;

        std::unique_ptr<GImage> image = LoadImageFromPath(textureDesc.path);

        if (!image)
        {
            std::cerr << "Cannot load image from path: " << textureDesc.path
                      << std::endl;
        }

        std::unique_ptr<GTexture> texture =
            GTexture::Create(*image, textureDesc.minFilter,
                textureDesc.magFilter, textureDesc.wrapS, textureDesc.wrapT);

        if (!texture)
        {
            std::cerr << "Cannot create texture: " << textureDesc.path
                      << std::endl;

            continue;
        }

        textures[textureKey] = std::move(texture);
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
        std::move(shaders), std::move(textures), std::move(triangleBuffer),
        std::move(quadBuffer));
}

}  // namespace galena
