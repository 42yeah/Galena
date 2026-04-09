#pragma once

#include "GEngineData.h"
#include "GHwBuffer.h"
#include "GShader.h"
#include "GTexture.h"

#include "Galena/GEngineDesc.h"

#include <GLES3/gl3.h>

#include <iostream>
#include <memory>
#include <unordered_map>

#include <cstdint>

namespace galena {

/* Distinction between engine data and engine resource:

   - Engine data is immutable pure data
   - Engine resources is created during runtime
*/

class GEngineResources
{
public:
    GEngineResources(GEngineData &&engineData,
        std::unordered_map<EGShaderKey, std::unique_ptr<GShader>> shaders,
        std::unordered_map<uint32_t, std::unique_ptr<GTexture>> textures,
        std::unique_ptr<GHwBuffer> triangleBuffer,
        std::unique_ptr<GHwBuffer> quadBuffer)
        : mEngineData(std::move(engineData)), mShaders(std::move(shaders)),
          mTextures(std::move(textures)),
          mTriangleBuffer(std::move(triangleBuffer)),
          mQuadBuffer(std::move(quadBuffer))
    {
    }

    GEngineResources(const GEngineResources &) = delete;

    GEngineResources(GEngineResources &&) = delete;

    ~GEngineResources();

public:
    static std::unique_ptr<GEngineResources> Create(const GEngineDesc &desc);

    GShader *Shader(EGShaderKey key) const { return mShaders.at(key).get(); }

    GTexture *Texture(uint32_t textureId) const
    {
        return mTextures.at(textureId).get();
    }

    template <typename F> bool CatchErrors(F &&scope) const
    {
        scope();

        GLenum err = glGetError();

        if (err == GL_NO_ERROR)
            return true;

        std::cout << "GL error: " << err << std::endl;

        return false;
    }

    GHwBuffer *TriangleBuffer() const { return mTriangleBuffer.get(); }

    GHwBuffer *QuadBuffer() const { return mQuadBuffer.get(); }

public:
    const GEngineData mEngineData;

    const std::unordered_map<EGShaderKey, std::unique_ptr<GShader>> mShaders;
    const std::unordered_map<uint32_t, std::unique_ptr<GTexture>> mTextures;

    const std::unique_ptr<GHwBuffer> mTriangleBuffer;
    const std::unique_ptr<GHwBuffer> mQuadBuffer;
};

}  // namespace galena
