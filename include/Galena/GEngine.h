#pragma once

#include "GEngineDesc.h"
#include "Galena/GRenderDesc.h"

#include <memory>

#include <cstdint>

namespace galena {

class GEngineResources;

class GEngine
{
public:
    GEngine(std::unique_ptr<GEngineResources> &&resources);

    ~GEngine();

public:
    static std::unique_ptr<GEngine> Create(const GEngineDesc &desc);

public:
    void SetRenderSurfaceSize(uint32_t width, uint32_t height);

    void Clear(float r, float g, float b, float a) const;

    void RenderDebugTriangle() const;

    bool RenderSprite(const GRenderSpriteDesc &spriteDesc) const;

    bool Render(const GRenderDesc &desc) const;

private:
    const std::unique_ptr<GEngineResources> mEngineResources;

    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
};

}  // namespace galena
