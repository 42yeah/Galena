#pragma once

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

    void SetX(float x) { mX = x; }

    float Y() const { return mY; }

    void SetY(float y) { mY = y; }

    float Width() const { return mWidth; }

    float Height() const { return mHeight; }

    float Vx() const { return mVx; }

    void SetVx(float vx) { mVx = vx; }

    float Vy() const { return mVy; }

    void SetVy(float vy) { mVy = vy; }

    float Ax() const { return mAx; }

    void SetAx(float ax) { mAx = ax; }

    float Ay() const { return mAy; }

    void SetAy(float ay) { mAy = ay; }

private:
    uint32_t mSpriteId = 0;

    float mX = 0.0f;
    float mY = 0.0f;
    float mWidth = 1.0f;
    float mHeight = 1.0f;

    float mVx = 0.0f;
    float mVy = 0.0f;

    float mAx = 0.0f;
    float mAy = 0.0f;
};

}  // namespace galena
