#pragma once

#include "GEngineDesc.h"
#include "GPostprocess.h"
#include "GRenderDesc.h"
#include "GTextureDesc.h"

#include <memory>

#include <cstdint>

namespace galena {

class GEngineResources;
class GFramebuffer;
class GPostprocessRenderer;
class GEngineState;

class GEngine
{
public:
    GEngine(std::unique_ptr<GEngineResources> &&resources);

    ~GEngine();

public:
    static std::unique_ptr<GEngine> Create(const GEngineDesc &desc);

public:
    uint32_t RenderWidth() const;

    uint32_t RenderHeight() const;

    void SetRenderSurfaceSize(uint32_t width, uint32_t height);

    void Clear(float r, float g, float b, float a) const;

    void RenderDebugTriangle() const;

    bool RenderPostprocess(GFramebuffer *pDstFramebuffer,
        GFramebuffer *pSrcFramebuffer, EGPostprocessType postprocType) const;

    bool Render(const GRenderDesc &desc) const;

    GFramebuffer *CreateFramebuffer(uint32_t width, uint32_t height,
        EGTextureFilter minFilter, EGTextureFilter magFilter);

    void ReleaseFramebuffer(GFramebuffer *pFramebuffer);

private:
    bool RenderSprite(uint32_t renderWidth, uint32_t renderHeight,
        const GRenderSpriteDesc &spriteDesc) const;

private:
    const std::unique_ptr<GEngineResources> mEngineResources;
    const std::unique_ptr<GEngineState> mEngineState;
    const std::unique_ptr<GPostprocessRenderer> mPostprocessRenderer;

    // Managed vector of framebuffers
    std::vector<std::unique_ptr<GFramebuffer>> mFramebuffers;
};

}  // namespace galena
