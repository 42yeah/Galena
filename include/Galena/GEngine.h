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

    void RenderBlock(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

private:
    const std::unique_ptr<GEngineResources> mEngineResources;
};

}  // namespace galena
