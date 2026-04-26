#include "Galena/GEngine.h"
#include "Galena/GEngineDesc.h"
#include "Galena/GPostprocess.h"
#include "Galena/GRenderDesc.h"
#include "Galena/GTextureDesc.h"

#include <emscripten/fetch.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>

#include <iostream>
#include <memory>

#include <cstdint>
#include <cstring>
#include <random>

using namespace galena;

#define GALENA_ITERATE_RES(Func) Func(Monde, "monde.png")

#define GALENA_ITERATE_FUNC(name, path) GSampleResource##name,

enum EGSampleResource
{
    GALENA_ITERATE_RES(GALENA_ITERATE_FUNC) GSampleResourceCount
};

#undef GALENA_ITERATE_FUNC

class Game
{
public:
    Game(std::unique_ptr<GEngine> &&engine) : mEngine(std::move(engine))
    {
        constexpr uint32_t SpriteSize = 64;

        GRenderSpriteDesc spriteDesc(
            GSampleResourceMonde, 0, 0, SpriteSize, SpriteSize, 0, 0, 16, 16);

        constexpr uint32_t RoomWidth = 5;
        constexpr uint32_t RoomHeight = 5;

        std::uniform_int_distribution<uint32_t> distrib(0, 8);
        std::random_device dev;

        for (size_t y = 0; y < RoomHeight; ++y)
        {
            for (size_t x = 0; x < RoomWidth; ++x)
            {
                bool isWall = false;

                if (x == 0)
                {
                    spriteDesc.sx = 16;
                    isWall = true;
                }
                else if (x == RoomWidth - 1)
                {
                    spriteDesc.sx = 48;
                    isWall = true;
                }
                else
                {
                    spriteDesc.sx = 32;
                }

                if (y == RoomHeight - 1)
                {
                    spriteDesc.sy = 0;
                    isWall = true;
                }
                else if (y == 0)
                {
                    spriteDesc.sy = 32;
                    isWall = true;
                }
                else
                {
                    spriteDesc.sy = 16;
                }

                if (!isWall)
                {
                    const uint32_t decorType = distrib(dev);

                    if (decorType == 0)
                    {
                        const uint32_t floorType = distrib(dev);
                        const uint32_t floorX = floorType % 3;
                        const uint32_t floorY = floorType / 3;

                        spriteDesc.sx = 64 + floorX * 16;
                        spriteDesc.sy = floorY * 16;
                    }
                    else if (decorType == 1)
                    {
                        const uint32_t furnitureType = distrib(dev);
                        const uint32_t furnitureX = furnitureType % 5;
                        const uint32_t furnitureY = furnitureType / 5;

                        spriteDesc.sx = 16 + furnitureX * 16;
                        spriteDesc.sy = 48 + furnitureY * 16;
                    }
                }

                spriteDesc.x = x * SpriteSize - 200;
                spriteDesc.y = y * SpriteSize - 200;

                mRenderDesc.spriteDescs.emplace_back(spriteDesc);
            }
        }

        mpGameFramebuffer = mEngine->CreateFramebuffer(
            300, 300, GTextureFilterLinear, GTextureFilterLinear);

        mRenderDesc.pDstFramebuffer = mpGameFramebuffer;
        mRenderDesc.clearColor = GColor(0.0f, 0.0f, 0.0f, 0.0f);
    }

public:
    void Loop()
    {
        mEngine->Render(mRenderDesc);

        mEngine->RenderPostprocess(
            nullptr, mpGameFramebuffer, galena::GPostprocessTypeBloom);
    }

private:
    std::unique_ptr<GEngine> mEngine = nullptr;

    GFramebuffer *mpGameFramebuffer = nullptr;

    GRenderDesc mRenderDesc;
};

std::unique_ptr<Game> gGame = nullptr;

void Loop() { gGame->Loop(); }

int32_t main()
{
    // First, fetch all resources to local

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context("#canvas", &attrs);

    emscripten_webgl_make_context_current(ctx);

    GEngineDesc desc;

#define GALENA_ITERATE_FUNC(name, path)                                        \
    desc.textures[GSampleResource##name] = GTextureDesc("/assets/" path,       \
        GTextureFilterNearest, GTextureFilterNearest, GTextureWrapModeRepeat,  \
        GTextureWrapModeRepeat);

    GALENA_ITERATE_RES(GALENA_ITERATE_FUNC)

#undef GALENA_ITERATE_FUNC

    std::unique_ptr<GEngine> engine = GEngine::Create(desc);
    if (!engine)
        return 1;

    engine->SetRenderSurfaceSize(300, 300);
    gGame = std::make_unique<Game>(std::move(engine));

    emscripten_set_main_loop(Loop, 0, true);

    return 0;
}
