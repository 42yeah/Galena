#include "GGameInstance.h"
#include "GGameObject.h"
#include "GGameTextureKey.h"

#include "Galena/GEngine.h"
#include "Galena/GEngineDesc.h"
#include "Galena/GRenderDesc.h"

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

        constexpr uint32_t RenderSize = 32;
        constexpr uint32_t SpriteSize = 16;

        constexpr uint32_t SpritesPerRow = 10;

        const uint32_t spriteId = gameObject.SpriteId();

        const uint32_t spriteRow = (spriteId / SpritesPerRow);
        const uint32_t spriteCol = (spriteId % SpritesPerRow);

        const uint32_t x = static_cast<uint32_t>(gameObject.X() * RenderSize);
        const uint32_t y = static_cast<uint32_t>(gameObject.Y() * RenderSize);

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

    mEngine->Clear(1.0f, 0.0f, 0.0f, 1.0f);
    isOk = mEngine->Render(renderDesc);

    if (!isOk)
    {
        std::cerr << "Failed to render" << std::endl;
        return false;
    }

    return true;
}

}  // namespace galena
