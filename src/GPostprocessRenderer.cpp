#include "GPostprocessRenderer.h"
#include "GEngineData.h"
#include "GFramebuffer.h"
#include "GShader.h"
#include "GTexture.h"

namespace galena {

template <typename F>
void BindFramebufferOrPresent(GFramebuffer *pFramebuffer, F &&scope)
{
    if (!pFramebuffer)
    {
        scope();
        return;
    }

    pFramebuffer->Bind(std::forward<F>(scope));
}

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
        GFramebuffer::CreateFramebuffer(width, height);

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

    pShader->Bind([&] {
        pSrcTexture->BindAndActive(0, [&] {
            glUniform1i(pShader->Location("sampleTexture"), 0);

            mpEngineResources->QuadBuffer()->Bind([&] {
                BindFramebufferOrPresent(
                    pDstFramebuffer, [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
            });
        });
    });

    return false;
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

    GShader *pAddShader = mpEngineResources->Shader(GShaderKeyAdd);
    if (!pAddShader)
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

        pAddShader->Bind([&] {
            pSrcTexture->BindAndActive(0, [&] {
                pCurrFramebuffer->FramebufferTexture()->BindAndActive(1, [&] {
                    glUniform1i(pAddShader->Location("textureA"), 0);
                    glUniform1i(pAddShader->Location("textureB"), 1);

                    BindFramebufferOrPresent(pDstFramebuffer,
                        [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
                });
            });
        });
    });

    return true;
}

}  // namespace galena
