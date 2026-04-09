#include "GTexture.h"

#include <GLES3/gl3.h>

#include <memory>

namespace galena {

GTexture::~GTexture()
{
    if (mTexture != GL_NONE)
        glDeleteTextures(1, &mTexture);
}

GLuint ToGlTextureFilter(EGTextureFilter filter)
{
    switch (filter)
    {
    case GTextureFilterNearest:
        return GL_NEAREST;

    case GTextureFilterLinear:
    default:
        return GL_LINEAR;
    }
}

GLuint ToGlTextureWrapMode(EGTextureWrapMode wrapMode)
{
    switch (wrapMode)
    {
    case GTextureWrapModeRepeat:
    default:
        return GL_REPEAT;

    case GTextureWrapModeClampToEdge:
        return GL_CLAMP_TO_EDGE;
    }
}

std::unique_ptr<GTexture> GTexture::Create(const GImage &image,
    EGTextureFilter minFilter, EGTextureFilter magFilter,
    EGTextureWrapMode wrapS, EGTextureWrapMode wrapT)
{
    GLuint texture = GL_NONE;

    glGenTextures(1, &texture);
    if (texture == GL_NONE)
        return nullptr;

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGlTextureFilter(minFilter));

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGlTextureFilter(magFilter));

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGlTextureWrapMode(wrapS));

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGlTextureWrapMode(wrapT));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, image.data.get());

    return std::make_unique<GTexture>(texture);
}

}  // namespace galena
