#include "Galena/GEngine.h"
#include "Galena/GVersion.h"

#include <GLES3/gl3.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <memory>

using namespace galena;

std::unique_ptr<GEngine> gEngine = nullptr;

void loop()
{
    gEngine->Clear(1.0f, 0.0f, 1.0f, 1.0f);
    gEngine->RenderDebugTriangle();
}

int32_t main()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context("#canvas", &attrs);

    emscripten_webgl_make_context_current(ctx);

    std::unique_ptr<GEngine> engine = GEngine::Create();

    // Elevate it to global scope
    gEngine = std::move(engine);

    emscripten_set_main_loop(loop, 0, true);
    
    return 0;
}
