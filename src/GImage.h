#pragma once

#include <functional>
#include <memory>

#include <cstdint>

namespace galena {

// For now, ALL images are RGBA8 unless stated otherwise

struct GImage
{
    using Deleter = void (*)(void *);

    std::unique_ptr<void, Deleter> data =
        std::unique_ptr<void, Deleter>(nullptr, [](void *) {});

    std::function<void()> dtor = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;

public:
    GImage() = default;

    GImage(void *pData, uint32_t width, uint32_t height)
        : data(pData, [](void *) {}), width(width), height(height)
    {
    }

    GImage(void *pData, Deleter dtor, uint32_t width, uint32_t height)
        : data(pData, dtor), width(width), height(height)
    {
    }

    ~GImage()
    {
        if (dtor)
            dtor();
    }
};

std::unique_ptr<GImage> LoadImageFromPath(const std::string &path);

}  // namespace galena
