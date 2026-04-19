#include "GGameInstance.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5_webgl.h>

#include <iostream>
#include <memory>

#include <cstdint>

using namespace galena;

std::unique_ptr<GGameInstance> gGameInstance = nullptr;

void Loop()
{
    thread_local double prevInstantInMs = 0.0;

    const double thisInstantInMs = emscripten_get_now();

    const float deltaTimeInSeconds =
        static_cast<float>((thisInstantInMs - prevInstantInMs) * 0.001);

    prevInstantInMs = thisInstantInMs;

    gGameInstance->Update(deltaTimeInSeconds);

    gGameInstance->Render();
}

bool OnKeyDown(
    int eventType, const EmscriptenKeyboardEvent *pKeyEvent, void *pUserData)
{
    return gGameInstance->UpdateInputState(pKeyEvent->keyCode, true);
}

bool OnKeyUp(
    int eventType, const EmscriptenKeyboardEvent *pKeyEvent, void *pUserData)
{
    return gGameInstance->UpdateInputState(pKeyEvent->keyCode, false);
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

    emscripten_set_keydown_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, OnKeyDown);

    emscripten_set_keyup_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, OnKeyUp);

    emscripten_set_main_loop(Loop, 30, true);

    return 0;
}
