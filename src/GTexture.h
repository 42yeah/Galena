#pragma once

#include "GImage.h"

#include <GLES3/gl3.h>

#include <memory>

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

class GTexture
{
public:
    explicit GTexture(GLuint texture) : mTexture(texture) {}

    ~GTexture();

public:
    static std::unique_ptr<GTexture> Create(const GImage &image,
        EGTextureFilter minFilter, EGTextureFilter magFilter,
        EGTextureWrapMode wrapS, EGTextureWrapMode wrapT);

public:
    template <typename F> void BindAndActive(size_t index, F &&scope)
    {
        glBindTexture(GL_TEXTURE_2D, mTexture);
        glActiveTexture(GL_TEXTURE0 + index);
    }

private:
    const GLuint mTexture = GL_NONE;
};

}  // namespace galena
