#pragma once

#include "Galena/GTextureDesc.h"

#include <unordered_map>

#include <cstdint>

namespace galena {

struct GEngineDesc
{
    std::unordered_map<uint32_t, GTextureDesc> textures;
};

}  // namespace galena
