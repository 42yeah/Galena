#include "Galena/GEngine.h"
#include "Galena/GPostprocess.h"
#include "Galena/GRenderDesc.h"

#include <bitset>
#include <emscripten/em_types.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>

#include <iostream>
#include <memory>
#include <optional>
#include <random>

#include <cstdint>

using namespace galena;

constexpr uint32_t GameFramebufferWidth = 150;
constexpr uint32_t GameFramebufferHeight = 150;

constexpr uint32_t CanvasWidth = 800;
constexpr uint32_t CanvasHeight = 800;

constexpr uint32_t MondeTexture = 0;
constexpr uint32_t SpriteSize = 16;
constexpr uint32_t SpriteRenderSize = 32;

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

            spriteDesc.x = x * SpriteRenderSize - 50;
            spriteDesc.y = y * SpriteRenderSize - 50;

            dstRenderDesc.spriteDescs.emplace_back(spriteDesc);
        }
    }
}

enum EGameKeyStateBit
{
    GameKeyStateLeft = 0,
    GameKeyStateDown,
    GameKeyStateRight,
    GameKeyStateUp,
    GameKeyStateCount,
};

class Game
{
public:
    Game(std::unique_ptr<GEngine> &&engine,
        GFramebuffer *const pGameFramebuffer, const GRenderDesc &&world)
        : mEngine(std::move(engine)), mpGameFramebuffer(pGameFramebuffer),
          mWorld(std::move(world))
    {
        // Don't clear since the last one already has
        mPlayerRenderDesc.pDstFramebuffer = mpGameFramebuffer;
        mPlayerRenderDesc.clearColor = std::nullopt;

        const GRenderSpriteDesc playerSpriteDesc(MondeTexture, 0, 0,
            SpriteRenderSize, SpriteRenderSize, 3 * SpriteSize, 5 * SpriteSize,
            SpriteSize, SpriteSize);

        mPlayerRenderDesc.spriteDescs.emplace_back(playerSpriteDesc);
    }

public:
    static std::unique_ptr<Game> Create()
    {
        GEngineDesc desc;
        desc.textures[MondeTexture] = "assets/monde.png";

        std::unique_ptr<GEngine> engine = GEngine::Create(desc);
        engine->SetRenderSurfaceSize(CanvasWidth, CanvasHeight);

        GFramebuffer *pGameFramebuffer = engine->CreateFramebuffer(
            GameFramebufferWidth, GameFramebufferHeight, GTextureFilterNearest,
            GTextureFilterNearest);

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
    void Update()
    {
        // Move the character around

        GRenderSpriteDesc &playerDesc = mPlayerRenderDesc.spriteDescs.front();

        constexpr float PlayerSpeed =
            static_cast<float>(SpriteRenderSize) * 2.0f;

        float speedX = 0.0f;
        float speedY = 0.0f;

        if (mKeyStates.test(GameKeyStateLeft))
            speedX = -1.0f;

        if (mKeyStates.test(GameKeyStateDown))
            speedY = -1.0f;

        if (mKeyStates.test(GameKeyStateRight))
            speedX = 1.0f;

        if (mKeyStates.test(GameKeyStateUp))
            speedY = 1.0f;

        // Normalize speed
        const float speedSqr = speedX * speedX + speedY * speedY;
        if (speedSqr < 0.001f)
            return;

        const float speedNor = 1.0f / sqrtf(speedSqr);
        const float norSpeedX = speedX * speedNor;
        const float norSpeedY = speedY * speedNor;

        mPlayerX += mDeltaTimeInSeconds * norSpeedX * PlayerSpeed;
        mPlayerY += mDeltaTimeInSeconds * norSpeedY * PlayerSpeed;

        playerDesc.x = static_cast<int32_t>(mPlayerX);
        playerDesc.y = static_cast<int32_t>(mPlayerY);

        if (speedX < 0.0f)
        {
            playerDesc.sx = 3 * SpriteSize;
        }
        else if (speedX > 0.0f)
        {
            playerDesc.sx = 4 * SpriteSize;
        }
    }

    void Loop()
    {
        const double thisInstant = emscripten_get_now();

        mDeltaTimeInSeconds =
            static_cast<float>((thisInstant - mPrevInstantInMs) * 0.001);

        mPrevInstantInMs = thisInstant;

        Update();

        mEngine->Clear(0.0f, 0.0f, 0.0f, 1.0f);
        mEngine->Render(mWorld);
        mEngine->Render(mPlayerRenderDesc);

        mEngine->RenderPostprocess(
            nullptr, mpGameFramebuffer, GPostprocessTypeCRT);
    }

    void SetKeyState(EGameKeyStateBit bit, bool isDown)
    {
        mKeyStates.set(bit, isDown);
    }

private:
    const std::unique_ptr<GEngine> mEngine;
    GFramebuffer *const mpGameFramebuffer;

    const GRenderDesc mWorld;

    GRenderDesc mPlayerRenderDesc;
    std::bitset<GameKeyStateCount> mKeyStates;

    double mPrevInstantInMs = 0.0;
    float mDeltaTimeInSeconds = 0.0f;

    float mPlayerX = 0.0f;
    float mPlayerY = 0.0f;
};

std::unique_ptr<Game> gGameInstance = nullptr;

void Loop() { gGameInstance->Loop(); }

constexpr uint32_t LeftKey = 65;   // A
constexpr uint32_t DownKey = 83;   // S
constexpr uint32_t RightKey = 68;  // D
constexpr uint32_t UpKey = 87;     // W

void SetGameKeyState(uint32_t keyCode, bool isDown)
{
    switch (keyCode)
    {
    case LeftKey:
        gGameInstance->SetKeyState(GameKeyStateLeft, isDown);
        break;

    case DownKey:
        gGameInstance->SetKeyState(GameKeyStateDown, isDown);
        break;

    case RightKey:
        gGameInstance->SetKeyState(GameKeyStateRight, isDown);
        break;

    case UpKey:
        gGameInstance->SetKeyState(GameKeyStateUp, isDown);
        break;

    default:
        break;
    }
}

bool OnKeyDown(
    int eventType, const EmscriptenKeyboardEvent *pKeyEvent, void *pUserData)
{
    SetGameKeyState(pKeyEvent->keyCode, true);
    return true;
}

bool OnKeyUp(
    int eventType, const EmscriptenKeyboardEvent *pKeyEvent, void *pUserData)
{
    SetGameKeyState(pKeyEvent->keyCode, false);
    return true;
}

int32_t main()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context("#canvas", &attrs);

    emscripten_webgl_make_context_current(ctx);

    gGameInstance = Game::Create();

    emscripten_set_keydown_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, OnKeyDown);

    emscripten_set_keyup_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, OnKeyUp);

    emscripten_set_main_loop(Loop, 30, true);

    return 0;
}
