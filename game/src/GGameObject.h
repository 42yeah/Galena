#pragma once

#include "GGameTextureKey.h"

#include <cstdint>

namespace galena {

class GGameObject
{
public:
    GGameObject() = default;

    GGameObject(uint32_t spriteId, float x, float y, float width, float height)
        : mSpriteId(spriteId)
    {
    }

public:
    uint32_t SpriteId() const { return mSpriteId; }

    float X() const { return mX; }

    float Y() const { return mY; }

    float Width() const { return mWidth; }

    float Height() const { return mHeight; }

private:
    uint32_t mSpriteId = 0;

    float mX = 0.0f;
    float mY = 0.0f;
    float mWidth = 1.0f;
    float mHeight = 1.0f;
};

}  // namespace galena
