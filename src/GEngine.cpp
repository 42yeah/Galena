#include "Galena/GEngine.h"

#include "GEngineData.h"
#include "GEngineResources.h"
#include "GShader.h"

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

void GEngine::RenderSprite(uint32_t textureId, uint32_t x, uint32_t y,
    uint32_t w, uint32_t h, uint32_t sx, uint32_t sy, uint32_t sw, uint32_t sh)
{
    GShader *pShader = mEngineResources->Shader(GShaderKeyTexturedQuad);

    mEngineResources->CatchErrors([&] {
        pShader->Bind([&] {
            glm::mat4 transform(1.0f);

            glUniformMatrix4fv(pShader->Location("transform"), 1, GL_FALSE,
                glm::value_ptr(transform));

            mEngineResources->Texture(textureId)->BindAndActive(0, [&] {
                glUniform1i(pShader->Location("sampleTexture"), 0);

                mEngineResources->QuadBuffer()->Bind([&] {
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                });
            });
        });
    });
}

}  // namespace galena
