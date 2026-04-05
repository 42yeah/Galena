#include "Galena/GEngine.h"

#include "GEngineData.h"
#include "GEngineResources.h"

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

std::unique_ptr<GEngine> GEngine::Create()
{
    std::unique_ptr<GEngineResources> resources = GEngineResources::Create();

    if (!resources)
    {
        std::cerr << "Failed to create engine resources" << std::endl;
        return nullptr;
    }

    std::unique_ptr<GEngine> engine =
        std::make_unique<GEngine>(std::move(resources));

    return engine;
}

void GEngine::RenderDebugTriangle()
{
    bool isOk = mEngineResources->CatchErrors([&] {
        mEngineResources->UseProgram(GShaderKeyDebug, [&] {
            mEngineResources->TriangleBuffer()->Bind(
                [&] { glDrawArrays(GL_TRIANGLES, 0, 3); });
        });
    });

    if (!isOk)
    {
        std::cerr << "Failed to render debug triangle" << std::endl;
    }
}

void GEngine::RenderBlock(
    uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
}

}  // namespace galena
