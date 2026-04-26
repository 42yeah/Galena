#pragma once

#include "GImage.h"

#include "Galena/GTextureDesc.h"

#include <GLES3/gl3.h>

#include <memory>

namespace galena {

class GTexture
{
public:
    explicit GTexture(GLuint texture, uint32_t width, uint32_t height)
        : mTexture(texture), mWidth(width), mHeight(height)
    {
    }

    ~GTexture();

public:
    static std::unique_ptr<GTexture> Create(const GImage &image,
        EGTextureFilter minFilter, EGTextureFilter magFilter,
        EGTextureWrapMode wrapS, EGTextureWrapMode wrapT);

    static std::unique_ptr<GTexture> Create(uint32_t width, uint32_t height,
        EGTextureFilter minFilter, EGTextureFilter magFilter,
        EGTextureWrapMode wrapS, EGTextureWrapMode wrapT);

public:
    template <typename F> void BindAndActive(size_t index, F &&scope)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        {
            scope();
        }

        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    GLuint Handle() const { return mTexture; }

    uint32_t Width() const { return mWidth; }

    uint32_t Height() const { return mHeight; }

private:
    const GLuint mTexture = GL_NONE;

    const uint32_t mWidth = 0;
    const uint32_t mHeight = 0;
};

}  // namespace galena
