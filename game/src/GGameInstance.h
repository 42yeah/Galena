#pragma once

#include "Galena/GEngine.h"
#include <memory>

namespace galena {

class GGameInstance
{
public:
    GGameInstance(std::unique_ptr<GEngine> &&engine)
        : mEngine(std::move(engine))
    {
    }

public:
    static std::unique_ptr<GGameInstance> Create();

public:
    void Update(float deltaTime);

    void Render();

private:
    const std::unique_ptr<GEngine> mEngine;

    float mDeltaTime = 0.0f;
};

}  // namespace galena
