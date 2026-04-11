#include "GImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#include <fstream>
#include <memory>

namespace galena {

std::unique_ptr<GImage> LoadStbImage(const void *pData, size_t size)
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t channels = 0;

    stbi_set_flip_vertically_on_load(false);

    stbi_uc *pDecodedData =
        stbi_load_from_memory(static_cast<const stbi_uc *>(pData), size, &width,
            &height, &channels, 0);

    if (!pDecodedData)
        return nullptr;

    std::unique_ptr<GImage> image = std::make_unique<GImage>(
        pDecodedData, [](void *pData) { stbi_image_free(pData); }, width,
        height);

    return image;
}

std::unique_ptr<GImage> LoadImageFromPath(const std::string &path)
{
    std::ifstream reader(path, std::ios::binary);

    if (!reader.good())
        return nullptr;

    reader.seekg(0, std::ios::end);
    size_t size = reader.tellg();

    reader.seekg(0, std::ios::beg);
    std::unique_ptr<char[]> data(new char[size]);

    reader.read(data.get(), size);

    return LoadStbImage(data.get(), size);
}

}  // namespace galena
