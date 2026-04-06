#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace galena {

struct GEngineDesc
{
    std::unordered_map<uint32_t, std::string> textures;
};

}  // namespace galena
