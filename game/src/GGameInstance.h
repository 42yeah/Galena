#pragma once

#include "GGameObject.h"

#include "Galena/GEngine.h"

#include <memory>

namespace galena {

class GGameInstance
{
public:
    GGameInstance(std::unique_ptr<GEngine> &&engine)
        : mEngine(std::move(engine))
    {
        GGameObject testGameObject(31, 0.0f, 0.0f, 1.0f, 1.0f);
        mGameObjects.emplace_back(testGameObject);
    }

public:
    static std::unique_ptr<GGameInstance> Create();

public:
    bool Update(float deltaTime);

    bool Render();

private:
    const std::unique_ptr<GEngine> mEngine;

    std::vector<GGameObject> mGameObjects;

    float mDeltaTime = 0.0f;
};

}  // namespace galena
