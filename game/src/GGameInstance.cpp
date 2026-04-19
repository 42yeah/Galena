#include "GGameInstance.h"
#include "GGameConstants.h"
#include "GGameInputState.h"
#include "GGameObject.h"
#include "GGameTextureKey.h"

#include "Galena/GEngine.h"
#include "Galena/GEngineDesc.h"
#include "Galena/GMath.h"
#include "Galena/GRenderDesc.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <type_traits>

namespace galena {

std::unique_ptr<GGameInstance> GGameInstance::Create()
{
    GEngineDesc desc;
    desc.textures[GGameTextureKeyMonde] = "assets/monde.png";

    std::unique_ptr<GEngine> engine = GEngine::Create(std::move(desc));
    engine->SetRenderSurfaceSize(300, 300);

    return std::make_unique<GGameInstance>(std::move(engine));
}

bool GGameInstance::Update(float deltaTime)
{
    mDeltaTime = deltaTime;

    bool isOk = UpdateControlObjectState();

    if (!isOk)
    {
        std::cerr << "Failed to update control object state" << std::endl;
    }

    for (GGameObject &gameObject : mGameObjects)
    {
        const float ax = gameObject.Ax();
        const float ay = gameObject.Ay();

        const float vx = gameObject.Vx();
        const float vy = gameObject.Vy();

        // First apply acceleration

        const float vx0 = Clamp(vx + ax * deltaTime, -SpeedCap, SpeedCap);
        const float vy0 = Clamp(vy + ay * deltaTime, -SpeedCap, SpeedCap);

        const float oldX = gameObject.X();
        const float oldY = gameObject.Y();

        float newX = oldX + vx0 * deltaTime;
        float newY = oldY + vy0 * deltaTime;

        const float frictionX = Sign(vx) * Friction;
        const float frictionY = Sign(vy) * Friction;

        float vx1 = vx0 - frictionX * deltaTime;
        float vy1 = vy0 - frictionY * deltaTime;

        if (vx1 * vx0 < 0.0f)
            vx1 = 0.0f;

        if (vy1 * vy0 < 0.0f)
            vy1 = 0.0f;

        gameObject.SetX(newX);
        gameObject.SetY(newY);

        gameObject.SetVx(vx1);
        gameObject.SetVy(vy1);
    }

    return true;
}

bool MakeRenderSpriteDesc(std::vector<GRenderSpriteDesc> &dstRenderSpriteDesc,
    const std::vector<GGameObject> &gameObjects)
{
    // For each game object, extract the render sprite desc

    dstRenderSpriteDesc.reserve(gameObjects.size());

    for (const GGameObject &gameObject : gameObjects)
    {
        // Translate to render sprite desc

        const uint32_t spriteId = gameObject.SpriteId();

        const uint32_t spriteRow = (spriteId / SpritesPerRow);
        const uint32_t spriteCol = (spriteId % SpritesPerRow);

        const int32_t x = static_cast<int32_t>(gameObject.X() * RenderSize);
        const int32_t y = static_cast<int32_t>(gameObject.Y() * RenderSize);

        const uint32_t width =
            static_cast<uint32_t>(gameObject.Width() * RenderSize);

        const uint32_t height =
            static_cast<uint32_t>(gameObject.Height() * RenderSize);

        GRenderSpriteDesc spriteDesc(GGameTextureKeyMonde, x, y, width, height,
            spriteCol * SpriteSize, spriteRow * SpriteSize, SpriteSize,
            SpriteSize);

        static_assert(std::is_trivially_copyable_v<GRenderSpriteDesc>,
            "This is a reminder to change this to std::move when it no longer "
            "becomes trivially copyable");

        dstRenderSpriteDesc.emplace_back(spriteDesc);
    }

    return true;
}

bool GGameInstance::Render()
{
    thread_local GRenderDesc renderDesc;
    renderDesc.pDstFramebuffer = nullptr;
    renderDesc.clearColor = GColor(1.0f, 0.0f, 0.0f, 1.0f);

    bool isOk = MakeRenderSpriteDesc(renderDesc.spriteDescs, mGameObjects);

    if (!isOk)
    {
        std::cerr << "Failed to make render desc" << std::endl;
        return false;
    }

    isOk = mEngine->Render(renderDesc);

    if (!isOk)
    {
        std::cerr << "Failed to render" << std::endl;
        return false;
    }

    return true;
}

bool GGameInstance::UpdateInputState(uint32_t keyCode, bool isDown)
{
    // TODO: Custom key mapping

    constexpr uint32_t LeftKey = 65;
    constexpr uint32_t DownKey = 83;
    constexpr uint32_t RightKey = 68;
    constexpr uint32_t UpKey = 87;

    // clang-format off
#define GALENA_INTERATE_INPUT_BINDINGS(Func) \
    Func(LeftKey, GGameInputStateLeft) \
    Func(DownKey, GGameInputStateDown) \
    Func(RightKey, GGameInputStateRight) \
    Func(UpKey, GGameInputStateUp)
    // clang-format on

#define GALENA_ITERATE_FUNC(key, action)                                       \
    case key:                                                                  \
        mInputStates.set(action, isDown);                                      \
        return true;

    switch (keyCode)
    {
        GALENA_INTERATE_INPUT_BINDINGS(GALENA_ITERATE_FUNC)

    default:
        break;
    }

#undef GALENA_ITERATE_FUNC

    return false;
}

bool GGameInstance::UpdateControlObjectState()
{
    GGameObject *pControlObject = ControlObject();
    if (!pControlObject)
        return false;

    float ax = 0.0f;
    float ay = 0.0f;

    if (mInputStates.test(GGameInputStateLeft))
        ax -= 1.0f;

    if (mInputStates.test(GGameInputStateDown))
        ay -= 1.0f;

    if (mInputStates.test(GGameInputStateRight))
        ax += 1.0f;

    if (mInputStates.test(GGameInputStateUp))
        ay += 1.0f;

    float length = sqrtf(ax * ax + ay * ay);

    if (length < Epsilon)
    {
        pControlObject->SetAx(0.0f);
        pControlObject->SetAy(0.0f);

        return true;
    }

    ax = (ax / length) * Acceleration;
    ay = (ay / length) * Acceleration;

    pControlObject->SetAx(ax);
    pControlObject->SetAy(ay);

    return true;
}

}  // namespace galena
