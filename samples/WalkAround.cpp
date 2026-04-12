#include "Galena/GEngine.h"
#include "Galena/GPostprocess.h"
#include "Galena/GRenderDesc.h"

#include <emscripten/html5_webgl.h>

#include <memory>
#include <random>

#include <cstdint>

using namespace galena;

constexpr uint32_t CanvasWidth = 300;
constexpr uint32_t CanvasHeight = 300;

constexpr uint32_t MondeTexture = 0;
constexpr uint32_t SpriteSize = 16;
constexpr uint32_t SpriteRenderSize = 64;

constexpr uint32_t RoomWidth = 6;
constexpr uint32_t RoomHeight = 6;

void GenerateWorld(GRenderDesc &dstRenderDesc, uint32_t width, uint32_t height)
{
    GRenderSpriteDesc spriteDesc(MondeTexture, 0, 0, SpriteRenderSize,
        SpriteRenderSize, 0, 0, SpriteSize, SpriteSize);

    std::uniform_int_distribution<uint32_t> distrib(0, 8);
    std::random_device dev;

    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
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

            spriteDesc.x = x * SpriteRenderSize - 200;
            spriteDesc.y = y * SpriteRenderSize - 200;

            dstRenderDesc.spriteDescs.emplace_back(spriteDesc);
        }
    }
}

class Game
{
public:
    Game(std::unique_ptr<GEngine> &&engine,
        GFramebuffer *const pGameFramebuffer, const GRenderDesc &&world)
        : mEngine(std::move(engine)), mpGameFramebuffer(pGameFramebuffer),
          mWorld(std::move(world))
    {
    }

public:
    static std::unique_ptr<Game> Create()
    {
        GEngineDesc desc;
        desc.textures[MondeTexture] = "assets/monde.png";

        std::unique_ptr<GEngine> engine = GEngine::Create(desc);
        engine->SetRenderSurfaceSize(CanvasWidth, CanvasHeight);

        GFramebuffer *pGameFramebuffer = engine->CreateFramebuffer(
            engine->RenderWidth(), engine->RenderHeight());

        if (!pGameFramebuffer)
            return nullptr;

        GRenderDesc renderDesc;
        renderDesc.pDstFramebuffer = pGameFramebuffer;
        renderDesc.clearColor = GColor(0.0f, 0.0f, 0.0f, 0.0f);
        GenerateWorld(renderDesc, RoomWidth, RoomHeight);

        return std::make_unique<Game>(
            std::move(engine), pGameFramebuffer, std::move(renderDesc));
    }

public:
    void Loop()
    {
        mEngine->Clear(0.0f, 0.0f, 0.0f, 1.0f);
        mEngine->Render(mWorld);

        mEngine->RenderPostprocess(
            nullptr, mpGameFramebuffer, GPostprocessTypeBloom);
    }

private:
    const std::unique_ptr<GEngine> mEngine;
    GFramebuffer *const mpGameFramebuffer;

    const GRenderDesc mWorld;
};

std::unique_ptr<Game> gGameInstance = nullptr;

void Loop() { gGameInstance->Loop(); }

int32_t main()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context("#canvas", &attrs);

    emscripten_webgl_make_context_current(ctx);

    gGameInstance = Game::Create();

    emscripten_set_main_loop(Loop, 0, true);

    return 0;
}
