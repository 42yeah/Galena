#include "GGameInstance.h"
#include "GGameTextureKey.h"
#include "Galena/GEngine.h"
#include "Galena/GEngineDesc.h"

#include <memory>

namespace galena {

std::unique_ptr<GGameInstance> GGameInstance::Create()
{
    GEngineDesc desc;
    desc.textures[GGameTextureKeyMonde] = "assets/monde.png";

    std::unique_ptr<GEngine> engine = GEngine::Create(std::move(desc));
    
    return std::make_unique<GGameInstance>(std::move(engine));
}

void GGameInstance::Update(float deltaTime)
{
    mDeltaTime = deltaTime;
}

void GGameInstance::Render()
{
    mEngine->Clear(1.0f, 0.0f, 0.0f, 1.0f);
    mEngine->RenderDebugTriangle();
}

}  // namespace galena
