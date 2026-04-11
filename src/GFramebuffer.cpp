#include "GFramebuffer.h"
#include "GTexture.h"

#include <GLES3/gl3.h>

#include <memory>

namespace galena {

GFramebuffer::~GFramebuffer()
{
    if (mFramebuffer != GL_NONE)
    {
        glDeleteFramebuffers(1, &mFramebuffer);
    }
}

std::unique_ptr<GFramebuffer> GFramebuffer::CreateFramebuffer(
    uint32_t width, uint32_t height)
{
    // To create a framebuffer, we must first create a framebuffer texture

    std::unique_ptr<GTexture> texture = GTexture::Create(width, height,
        GTextureFilterLinear, GTextureFilterLinear, GTextureWrapModeRepeat,
        GTextureWrapModeRepeat);

    if (!texture)
        return nullptr;

    GLuint framebuffer = GL_NONE;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    texture->BindAndActive(0, [&] {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, texture->Handle(), 0);
    });

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return nullptr;

    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    return std::make_unique<GFramebuffer>(std::move(texture), framebuffer);
}

}  // namespace galena
