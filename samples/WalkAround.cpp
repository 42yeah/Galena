#include "Galena/GEngine.h"

#include <emscripten/html5_webgl.h>

#include <memory>

#include <cstdint>

using namespace galena;

constexpr uint32_t MondeTexture = 0;

class Game
{
public:
    Game(std::unique_ptr<GEngine> &&engine) : mEngine(std::move(engine)) {}

public:
    static std::unique_ptr<Game> Create()
    {
        GEngineDesc desc;
        desc.textures[MondeTexture] = "assets/monde.png";

        std::unique_ptr<GEngine> engine = GEngine::Create(desc);

        return std::make_unique<Game>(std::move(engine));
    }

public:
    void Loop()
    {
        mEngine->Clear(0.0f, 0.0f, 0.0f, 1.0f);
        mEngine->RenderDebugTriangle();
    }

private:
    const std::unique_ptr<GEngine> mEngine;
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
