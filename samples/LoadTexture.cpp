#include "Galena/GEngine.h"
#include "Galena/GEngineDesc.h"
#include <cstdint>

#include <cstring>
#include <emscripten/fetch.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>

#include <memory>

using namespace galena;

#define GALENA_ITERATE_RES(Func) Func(Monde, "monde.png")

#define GALENA_ITERATE_FUNC(name, path) GSampleResource##name,

enum EGSampleResource
{
    GALENA_ITERATE_RES(GALENA_ITERATE_FUNC) GSampleResourceCount
};

#undef GALENA_ITERATE_FUNC

std::unique_ptr<GEngine> gEngine = nullptr;

void Loop() { gEngine->Clear(1.0f, 0.5f, 0.0f, 1.0f); }

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
    desc.textures[GSampleResource##name] = "/assets/" path;

    GALENA_ITERATE_RES(GALENA_ITERATE_FUNC)

#undef GALENA_ITERATE_FUNC

    std::unique_ptr<GEngine> engine = GEngine::Create(desc);
    if (!engine)
        return 1;

    gEngine = std::move(engine);

    emscripten_set_main_loop(Loop, 0, true);

    return 0;
}
