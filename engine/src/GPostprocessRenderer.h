#pragma once

#include "GEngineData.h"
#include "GEngineResources.h"
#include "GEngineState.h"
#include "GFramebuffer.h"

#include "GShader.h"
#include "GTexture.h"
#include "Galena/GPostprocess.h"

#include <memory>

#include <cstdint>

namespace galena {

// Postprocess renderer with some supported effects

class GPostprocessRenderer
{
public:
    GPostprocessRenderer(const GEngineResources &engineResources,
        const GEngineState &pEngineState)
        : mpEngineResources(&engineResources), mpEngineState(&pEngineState)
    {
    }

public:
    bool RenderPostprocess(GFramebuffer *pDstFramebuffer,
        GFramebuffer *pSrcFramebuffer, EGPostprocessType postprocType);

private:
    GFramebuffer *AcquireNextFramebuffer(uint32_t width, uint32_t height);

    // Simple shaders with no uniforms required
    bool RenderPostprocessSimple(GFramebuffer *pDstFramebuffer,
        GTexture *pSrcTexture, EGShaderKey shaderKey);

    bool RenderPostprocessBloom(GFramebuffer *pDstFramebuffer,
        GTexture *pSrcTexture, uint32_t numIterations);

    bool RenderPostprocessCRT(
        GFramebuffer *pDstFramebuffer, GTexture *pSrcTexture);

private:
    const GEngineResources *const mpEngineResources;
    const GEngineState *const mpEngineState;

    std::unique_ptr<GFramebuffer> mPingpongFramebuffers[2];
    uint32_t mCurrPingpongIdx = 0;
};

}  // namespace galena
