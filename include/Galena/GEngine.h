#pragma once

#include "GEngineDesc.h"

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
    void Clear(float r, float g, float b, float a);

    void RenderDebugTriangle();

    void RenderSprite(uint32_t spriteId, uint32_t x, uint32_t y, uint32_t w,
        uint32_t h, uint32_t sx, uint32_t sy, uint32_t sw, uint32_t sh);

private:
    const std::unique_ptr<GEngineResources> mEngineResources;
};

}  // namespace galena
