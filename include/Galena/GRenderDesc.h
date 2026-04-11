#pragma once

#include <optional>
#include <vector>

#include <cstdint>

namespace galena {

class GFramebuffer;

struct GRenderSpriteDesc
{
    uint32_t textureId = 0;
    int32_t x = 0;
    int32_t y = 0;
    uint32_t w = 0;
    uint32_t h = 0;

    uint32_t sx = 0;
    uint32_t sy = 0;
    uint32_t sw = 0;
    uint32_t sh = 0;

public:
    GRenderSpriteDesc() = default;

    GRenderSpriteDesc(uint32_t textureId, int32_t x, int32_t y, uint32_t w,
        uint32_t h, uint32_t sx, uint32_t sy, uint32_t sw, uint32_t sh)
        : textureId(textureId), x(x), y(y), w(w), h(h), sx(sx), sy(sy), sw(sw),
          sh(sh)
    {
    }
};

struct GColor
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

public:
    GColor() = default;

    GColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

struct GRenderDesc
{
    GFramebuffer *pDstFramebuffer = nullptr;
    std::optional<GColor> clearColor = std::nullopt;

    std::vector<GRenderSpriteDesc> spriteDescs;
};

}  // namespace galena
