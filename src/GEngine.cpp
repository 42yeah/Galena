#include "Galena/GEngine.h"

#include "GEngineData.h"
#include "GEngineResources.h"
#include "GFramebuffer.h"
#include "GHwBuffer.h"
#include "GPostprocessRenderer.h"
#include "GRenderUtils.h"
#include "GShader.h"
#include "GTexture.h"

#include "Galena/GPostprocess.h"
#include "Galena/GRenderDesc.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLES3/gl3.h>

#include <algorithm>
#include <iostream>
#include <memory>

namespace galena {

GEngine::GEngine(std::unique_ptr<GEngineResources> &&resources)
    : mEngineResources(std::move(resources)),
      mPostprocessRenderer(
          std::make_unique<GPostprocessRenderer>(mEngineResources.get()))
{
}

GEngine::~GEngine() {}

std::unique_ptr<GEngine> GEngine::Create(const GEngineDesc &desc)
{
    glEnable(GL_BLEND);

    std::unique_ptr<GEngineResources> resources =
        GEngineResources::Create(desc);

    if (!resources)
    {
        std::cerr << "Failed to create engine resources" << std::endl;
        return nullptr;
    }

    std::unique_ptr<GEngine> engine =
        std::make_unique<GEngine>(std::move(resources));

    return std::move(engine);
}

void GEngine::SetRenderSurfaceSize(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;
}

void GEngine::Clear(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GEngine::RenderDebugTriangle() const
{
    GShader *pShader = mEngineResources->Shader(GShaderKeyDebug);

    pShader->Bind([&] {
        glm::mat4 transform(1.0f);

        glUniformMatrix4fv(pShader->Location("transform"), 1, GL_FALSE,
            glm::value_ptr(transform));

        mEngineResources->TriangleBuffer()->Bind(
            [&] { glDrawArrays(GL_TRIANGLES, 0, 3); });
    });
}

bool GEngine::RenderSprite(const GRenderSpriteDesc &spriteDesc) const
{
    GShader *pShader = mEngineResources->Shader(GShaderKeyTexturedQuad);
    GTexture *pTexture = mEngineResources->Texture(spriteDesc.textureId);

    if (!pShader || !pTexture)
        return false;

    const glm::vec2 surfaceSize =
        glm::vec2(static_cast<float>(mWidth), static_cast<float>(mHeight));

    if (surfaceSize.x == 0.0f || surfaceSize.y == 0.0)
        return false;

    const glm::vec2 position = glm::vec2(static_cast<float>(spriteDesc.x),
                                   static_cast<float>(spriteDesc.y)) /
                               surfaceSize;

    const glm::vec2 size = 0.5f *
                           glm::vec2(static_cast<float>(spriteDesc.w),
                               static_cast<float>(spriteDesc.h)) /
                           surfaceSize;

    const glm::vec2 textureSize =
        glm::vec2(static_cast<float>(pTexture->Width()),
            static_cast<float>(pTexture->Height()));

    if (textureSize.x == 0.0f || textureSize.y == 0.0f)
        return false;

    const glm::vec2 subposition = glm::vec2(static_cast<float>(spriteDesc.sx),
                                      static_cast<float>(spriteDesc.sy)) /
                                  textureSize;

    const glm::vec2 subsize = glm::vec2(static_cast<float>(spriteDesc.sw),
                                  static_cast<float>(spriteDesc.sh)) /
                              textureSize;

    if (subsize.x == 0.0f || subsize.y == 0.0f)
        return false;

    pShader->Bind([&] {
        glm::mat4 transform(1.0f);

        transform = glm::translate(transform, glm::vec3(position, 1.0f));
        transform = glm::scale(transform, glm::vec3(size, 1.0f));

        glm::mat4 sampleTransform(1.0f);

        sampleTransform =
            glm::translate(sampleTransform, glm::vec3(subposition, 0.0f));

        sampleTransform = glm::scale(sampleTransform, glm::vec3(subsize, 1.0f));

        glUniformMatrix4fv(pShader->Location("transform"), 1, GL_FALSE,
            glm::value_ptr(transform));

        glUniformMatrix4fv(pShader->Location("sampleTransform"), 1, GL_FALSE,
            glm::value_ptr(sampleTransform));

        pTexture->BindAndActive(0, [&] {
            glUniform1i(pShader->Location("sampleTexture"), 0);

            mEngineResources->QuadBuffer()->Bind(
                [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
        });
    });

    return true;
}

bool GEngine::RenderPostprocess(GFramebuffer *pDstFramebuffer,
    GFramebuffer *pSrcFramebuffer, EGPostprocessType postprocType) const
{
    return mPostprocessRenderer->RenderPostprocess(
        pDstFramebuffer, pSrcFramebuffer, postprocType);
}

bool GEngine::Render(const GRenderDesc &desc) const
{
    bool isOk = true;

    if (desc.clearColor.has_value())
    {
        const GColor &color = *desc.clearColor;

        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    BindFramebufferOrPresent(desc.pDstFramebuffer, [&] {
        for (const GRenderSpriteDesc &spriteDesc : desc.spriteDescs)
        {
            isOk = isOk && RenderSprite(spriteDesc);
        }
    });

    return isOk;
}

GFramebuffer *GEngine::CreateFramebuffer(uint32_t width, uint32_t height)
{
    std::unique_ptr<GFramebuffer> framebuffer =
        GFramebuffer::CreateFramebuffer(width, height);

    if (!framebuffer)
        return nullptr;

    mFramebuffers.emplace_back(std::move(framebuffer));

    return mFramebuffers.back().get();
}

void GEngine::ReleaseFramebuffer(GFramebuffer *pFramebuffer)
{
    auto cit = std::find_if(mFramebuffers.cbegin(), mFramebuffers.cend(),
        [pFramebuffer](const std::unique_ptr<GFramebuffer> &framebuffer) {
            return framebuffer.get() == pFramebuffer;
        });

    // Double free maybe? In any case we did not produce this
    if (cit == mFramebuffers.cend())
        return;

    mFramebuffers.erase(cit);
}

}  // namespace galena
