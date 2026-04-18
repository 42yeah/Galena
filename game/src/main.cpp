#include "GGameInstance.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5_webgl.h>

#include <memory>
#include <iostream>

#include <cstdint>

using namespace galena;

std::unique_ptr<GGameInstance> gGameInstance = nullptr;

void Loop()
{
    thread_local double prevInstantInMs = 0.0;
    
    const double thisInstant = emscripten_get_now();
    const float deltaTimeInSeconds = static_cast<float>((thisInstant - prevInstantInMs) * 0.001); 
    
    gGameInstance->Update(deltaTimeInSeconds);

    gGameInstance->Render();
}

int32_t main()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context("#canvas", &attrs);

    emscripten_webgl_make_context_current(ctx);

    gGameInstance = GGameInstance::Create();

    if (!gGameInstance)
    {
        std::cerr << "Failed to create game instance" << std::endl;
        return 1;
    }

    emscripten_set_main_loop(Loop, 30, true);

    return 0;
}
