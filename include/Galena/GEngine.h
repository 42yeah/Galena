#pragma once

#include "GEngineDesc.h"

#include "Galena/GPostprocess.h"
#include "Galena/GRenderDesc.h"

#include <memory>

#include <cstdint>

namespace galena {

class GEngineResources;
class GFramebuffer;
class GPostprocessRenderer;

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

    bool RenderPostprocess(GFramebuffer *pDstFramebuffer,
        GFramebuffer *pSrcFramebuffer, EGPostprocessType postprocType) const;

    bool Render(const GRenderDesc &desc) const;

    GFramebuffer *CreateFramebuffer(uint32_t width, uint32_t height);

    void ReleaseFramebuffer(GFramebuffer *pFramebuffer);

private:
    const std::unique_ptr<GEngineResources> mEngineResources;
    const std::unique_ptr<GPostprocessRenderer> mPostprocessRenderer;

    uint32_t mWidth = 0;
    uint32_t mHeight = 0;

    // Managed vector of framebuffers
    std::vector<std::unique_ptr<GFramebuffer>> mFramebuffers;
};

}  // namespace galena
