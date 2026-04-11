#include "Galena/GEngine.h"

#include "GEngineData.h"
#include "GEngineResources.h"
#include "GShader.h"
#include "GTexture.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLES3/gl3.h>

#include <iostream>
#include <memory>

namespace galena {

GEngine::GEngine(std::unique_ptr<GEngineResources> &&resources)
    : mEngineResources(std::move(resources))
{
}

GEngine::~GEngine() {}

void GEngine::Clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GEngine::SetRenderSurfaceSize(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;
}

std::unique_ptr<GEngine> GEngine::Create(const GEngineDesc &desc)
{
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

void GEngine::RenderDebugTriangle()
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

bool GEngine::RenderSprite(uint32_t textureId, int32_t x, int32_t y, uint32_t w,
    uint32_t h, uint32_t sx, uint32_t sy, uint32_t sw, uint32_t sh)
{
    GShader *pShader = mEngineResources->Shader(GShaderKeyTexturedQuad);
    GTexture *pTexture = mEngineResources->Texture(textureId);

    if (!pShader || !pTexture)
        return false;

    const glm::vec2 surfaceSize =
        glm::vec2(static_cast<float>(mWidth), static_cast<float>(mHeight));

    if (surfaceSize.x == 0.0f || surfaceSize.y == 0.0)
        return false;

    const glm::vec2 position =
        glm::vec2(static_cast<float>(x), static_cast<float>(y)) / surfaceSize;

    const glm::vec2 size =
        glm::vec2(static_cast<float>(w), static_cast<float>(h)) / surfaceSize;

    const glm::vec2 textureSize =
        glm::vec2(static_cast<float>(pTexture->Width()),
            static_cast<float>(pTexture->Height()));

    if (textureSize.x == 0.0f || textureSize.y == 0.0f)
        return false;

    const glm::vec2 subposition =
        glm::vec2(static_cast<float>(sx), static_cast<float>(sy)) / textureSize;

    const glm::vec2 subsize =
        glm::vec2(static_cast<float>(sw), static_cast<float>(sh)) / textureSize;

    if (subsize.x == 0.0f || subsize.y == 0.0f)
        return false;

    pShader->Bind([&] {
        glm::mat4 transform(1.0f);

        transform = glm::translate(transform, glm::vec3(position, 1.0f));
        transform = glm::scale(transform, glm::vec3(size, 1.0f));

        glm::mat4 sampleTransform(1.0f);

        sampleTransform = glm::translate(
            sampleTransform, glm::vec3(subposition, 0.0f));

        sampleTransform = glm::scale(sampleTransform, glm::vec3(subsize, 1.0f));

        glUniformMatrix4fv(pShader->Location("transform"), 1, GL_FALSE,
            glm::value_ptr(transform));

        glUniformMatrix4fv(pShader->Location("sampleTransform"), 1, GL_FALSE,
            glm::value_ptr(sampleTransform));

        mEngineResources->Texture(textureId)->BindAndActive(0, [&] {
            glUniform1i(pShader->Location("sampleTexture"), 0);

            mEngineResources->QuadBuffer()->Bind(
                [&] { glDrawArrays(GL_TRIANGLES, 0, 6); });
        });
    });

    return true;
}

}  // namespace galena
