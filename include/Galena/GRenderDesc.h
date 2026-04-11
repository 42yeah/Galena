#pragma once

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

struct GRenderDesc
{
    GFramebuffer *pDstFramebuffer = nullptr;

    std::vector<GRenderSpriteDesc> spriteDescs;
};

}  // namespace galena
