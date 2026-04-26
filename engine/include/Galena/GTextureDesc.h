#pragma once

#include <string>

namespace galena {

enum EGTextureFilter
{
    GTextureFilterNearest = 0,
    GTextureFilterLinear
};

enum EGTextureWrapMode
{
    GTextureWrapModeRepeat = 0,
    GTextureWrapModeClampToEdge
};

struct GTextureDesc
{
    std::string path;

    EGTextureFilter minFilter = GTextureFilterLinear;
    EGTextureFilter magFilter = GTextureFilterLinear;

    EGTextureWrapMode wrapS = GTextureWrapModeRepeat;
    EGTextureWrapMode wrapT = GTextureWrapModeRepeat;

public:
    GTextureDesc() = default;

    GTextureDesc(std::string path, EGTextureFilter minFilter,
        EGTextureFilter magFilter, EGTextureWrapMode wrapS,
        EGTextureWrapMode wrapT)
        : path(std::move(path)), minFilter(minFilter), magFilter(magFilter),
          wrapS(wrapS), wrapT(wrapT)
    {
    }
};

}  // namespace galena
