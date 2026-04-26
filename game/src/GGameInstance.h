#pragma once

#include "GGameInputState.h"
#include "GGameObject.h"

#include "Galena/GEngine.h"

#include <bitset>
#include <memory>

namespace galena {

class GGameInstance
{
public:
    GGameInstance(std::unique_ptr<GEngine> &&engine, GFramebuffer *pFramebuffer)
        : mEngine(std::move(engine)), mpFramebuffer(pFramebuffer)
    {
        GGameObject testGameObject(32, 0.0f, 0.0f, 1.0f, 1.0f);
        mGameObjects.emplace_back(testGameObject);  // Me
    }

public:
    static std::unique_ptr<GGameInstance> Create(
        uint32_t initialWidth, uint32_t initialHeight);

public:
    bool Update(float deltaTime);

    bool Render();

    bool UpdateInputState(uint32_t keyCode, bool isDown);

private:
    bool UpdateControlObjectState();

private:
    GGameObject *ControlObject()
    {
        if (mGameObjects.size() <= mControlIdx)
            return nullptr;

        return &mGameObjects[mControlIdx];
    }

private:
    const std::unique_ptr<GEngine> mEngine;
    GFramebuffer *const mpFramebuffer;

    std::vector<GGameObject> mGameObjects;
    uint32_t mControlIdx = 0;

    std::bitset<GGameInputStateCount> mInputStates;

    float mDeltaTime = 0.0f;
};

}  // namespace galena
