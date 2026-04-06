#pragma once

#include <functional>
#include <memory>

#include <cstdint>

namespace galena {

// For now, ALL images are RGBA8 unless stated otherwise

struct GImage
{
    const void *pData = nullptr;
    std::function<void()> dtor = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;

public:
    GImage() = default;

    GImage(const void *pData, std::function<void()> dtor, uint32_t width,
        uint32_t height)
        : pData(pData), dtor(std::move(dtor)), width(width), height(height)
    {
    }

    ~GImage()
    {
        if (dtor)
            dtor();
    }
};

std::unique_ptr<GImage> LoadPngImage(const void *pData, size_t size);

}  // namespace galena
