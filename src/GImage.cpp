#include "GImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#include <memory>

namespace galena {

std::unique_ptr<GImage> LoadStbImage(const void *pData, size_t size)
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t channels = 0;

    stbi_uc *pDecodedData =
        stbi_load_from_memory(static_cast<const stbi_uc *>(pData), size, &width,
            &height, &channels, 0);

    if (!pDecodedData)
        return nullptr;

    std::unique_ptr<GImage> image = std::make_unique<GImage>(
        pDecodedData, [pDecodedData]() { stbi_image_free(pDecodedData); },
        width, height);

    return image;
}

std::unique_ptr<GImage> LoadPngImage(const void *pData, size_t size)
{
    return LoadStbImage(pData, size);
}

}  // namespace galena
