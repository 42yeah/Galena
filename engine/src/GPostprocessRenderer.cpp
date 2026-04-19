#include "GPostprocessRenderer.h"
#include "GEngineData.h"
#include "GFramebuffer.h"
#include "GRenderUtils.h"
#include "GShader.h"
#include "GTexture.h"
#include "Galena/GPostprocess.h"

#include <GLES3/gl3.h>

namespace galena {

constexpr uint32_t NumGaussianBlurIterations = 5;

bool GPostprocessRenderer::RenderPostprocess(GFramebuffer *pDstFramebuffer,
    GFramebuffer *pSrcFramebuffer, EGPostprocessType postprocType)
{
    if (!pSrcFramebuffer)
        return false;

    GTexture *pTexture = pSrcFramebuffer->FramebufferTexture();
    if (!pTexture)
        return false;

    switch (postprocType)
    {
    case GPostprocessTypeInvert:
        return RenderPostprocessSimple(
            pDstFramebuffer, pTexture, GShaderKeyInvert);

    case GPostprocessTypeBloom:
        // Complex postprocesses gets their own functions
        return RenderPostprocessBloom(
            pDstFramebuffer, pTexture, NumGaussianBlurIterations);

    case GPostprocessTypeCRT:
        return RenderPostprocessCRT(pDstFramebuffer, pTexture);

    default:
        return false;
    }
}

GFramebuffer *GPostprocessRenderer::AcquireNextFramebuffer(
    uint32_t width, uint32_t height)
{
    const uint32_t currPingpongIdx = mCurrPingpongIdx;
    mCurrPingpongIdx = 1 - mCurrPingpongIdx;

    GFramebuffer *const pAvailableFramebuffer =
        mPingpongFramebuffers[currPingpongIdx].get();

    const bool needMatch =
        (!pAvailableFramebuffer || pAvailableFramebuffer->Width() != width ||
            pAvailableFramebuffer->Height() != height);

    if (!needMatch)
        return pAvailableFramebuffer;

    std::unique_ptr<GFramebuffer> newFramebuffer =
        GFramebuffer::CreateFramebuffer(
            width, height, GTextureFilterLinear, GTextureFilterLinear);

    GFramebuffer *pNewFramebuffer = newFramebuffer.get();
    mPingpongFramebuffers[currPingpongIdx] = std::move(newFramebuffer);

    return pNewFramebuffer;
}

bool GPostprocessRenderer::RenderPostprocessSimple(
    GFramebuffer *pDstFramebuffer, GTexture *pSrcTexture, EGShaderKey shaderKey)
{
    if (!pSrcTexture)
        return false;

    GShader *pShader = mpEngineResources->Shader(shaderKey);
    if (!pShader)
        return false;

    const uint32_t renderWidth = mpEngineState->renderWidth;
    const uint32_t renderHeight = mpEngineState->renderHeight;

    pShader->Bind([&] {
        pSrcTexture->BindAndActive(0, [&] {
            glUniform1i(pShader->Location("sampleTexture"), 0);

            mpEngineResources->QuadBuffer()->Bind([&] {
                BindFramebufferOrPresent(pDstFramebuffer, renderWidth,
                    renderHeight, [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
            });
        });
    });

    return true;
}

bool GPostprocessRenderer::RenderPostprocessBloom(GFramebuffer *pDstFramebuffer,
    GTexture *pSrcTexture, uint32_t numIterations)
{
    if (!pSrcTexture)
        return false;

    const uint32_t width = pSrcTexture->Width();
    const uint32_t height = pSrcTexture->Height();

    GShader *pGaussianBlurShader =
        mpEngineResources->Shader(GShaderKeyGaussianBlur);

    if (!pGaussianBlurShader)
        return false;

    GShader *pBlendShader = mpEngineResources->Shader(GShaderKeyBlend);
    if (!pBlendShader)
        return false;

    GTexture *pCurrTexture = pSrcTexture;

    mpEngineResources->QuadBuffer()->Bind([&] {
        GFramebuffer *pCurrFramebuffer = AcquireNextFramebuffer(width, height);

        pGaussianBlurShader->Bind([&] {
            for (size_t i = 0; i < numIterations; ++i)
            {
                if (i != 0)
                {
                    pCurrTexture = pCurrFramebuffer->FramebufferTexture();
                    pCurrFramebuffer = AcquireNextFramebuffer(width, height);
                }

                const GLuint sampleTextureLocation =
                    pGaussianBlurShader->Location("sampleTexture");

                const GLuint isHorizontalLocation =
                    pGaussianBlurShader->Location("horizontal");

                glUniform1i(isHorizontalLocation, true);
                glUniform1i(sampleTextureLocation, 0);

                pCurrTexture->BindAndActive(0, [&] {
                    pCurrFramebuffer->Bind(
                        [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
                });

                pCurrTexture = pCurrFramebuffer->FramebufferTexture();
                pCurrFramebuffer = AcquireNextFramebuffer(width, height);

                glUniform1i(isHorizontalLocation, false);
                pCurrTexture->BindAndActive(0, [&] {
                    pCurrFramebuffer->Bind(
                        [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
                });
            }
        });

        const uint32_t renderWidth = mpEngineState->renderWidth;
        const uint32_t renderHeight = mpEngineState->renderHeight;

        pBlendShader->Bind([&] {
            pSrcTexture->BindAndActive(0, [&] {
                pCurrFramebuffer->FramebufferTexture()->BindAndActive(1, [&] {
                    glUniform1i(pBlendShader->Location("textureA"), 0);
                    glUniform1i(pBlendShader->Location("textureB"), 1);

                    BindFramebufferOrPresent(pDstFramebuffer, renderWidth,
                        renderHeight,
                        [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
                });
            });
        });
    });

    return true;
}

bool GPostprocessRenderer::RenderPostprocessCRT(
    GFramebuffer *pDstFramebuffer, GTexture *pSrcTexture)
{
    if (!pSrcTexture)
        return false;

    GShader *pCRTShader = mpEngineResources->Shader(GShaderKeyCRT);
    if (!pCRTShader)
        return false;

    float targetWidth = mpEngineState->renderWidth;
    float targetHeight = mpEngineState->renderHeight;

    if (pDstFramebuffer)
    {
        targetWidth = static_cast<float>(pDstFramebuffer->Width());
        targetHeight = static_cast<float>(pDstFramebuffer->Height());
    }

    const uint32_t renderWidth = mpEngineState->renderWidth;
    const uint32_t renderHeight = mpEngineState->renderHeight;

    BindFramebufferOrPresent(pDstFramebuffer, renderWidth, renderHeight, [&] {
        mpEngineResources->QuadBuffer()->Bind([&] {
            pCRTShader->Bind([&] {
                pSrcTexture->BindAndActive(0, [&] {
                    glUniform1i(pCRTShader->Location("sampleTexture"), 0);

                    glUniform2f(pCRTShader->Location("framebufferResolution"),
                        targetWidth, targetHeight);

                    glDrawArrays(GL_TRIANGLES, 0, 6);
                });
            });
        });
    });

    return true;

    if (!pSrcTexture)
        return false;

    return true;
}

}  // namespace galena
