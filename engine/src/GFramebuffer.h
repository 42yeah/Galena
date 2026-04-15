#pragma once

#include "GTexture.h"
#include <GLES3/gl3.h>
#include <memory>
namespace galena {

class GFramebuffer
{
public:
    GFramebuffer(std::unique_ptr<GTexture> &&texture, GLuint framebuffer)
        : mFramebufferTexture(std::move(texture)), mFramebuffer(framebuffer)
    {
    }

    ~GFramebuffer();

public:
    static std::unique_ptr<GFramebuffer> CreateFramebuffer(uint32_t width,
        uint32_t height, EGTextureFilter minFilter, EGTextureFilter magFilter);

public:
    template <typename F> void Bind(F &&scope)
    {
        GLint oldViewports[4];
        glGetIntegerv(GL_VIEWPORT, oldViewports);

        glViewport(0, 0, Width(), Height());
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        {
            scope();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

        glViewport(
            oldViewports[0], oldViewports[1], oldViewports[2], oldViewports[3]);
    }

    GTexture *FramebufferTexture() const { return mFramebufferTexture.get(); }

    uint32_t Width() const { return mFramebufferTexture->Width(); }

    uint32_t Height() const { return mFramebufferTexture->Height(); }

private:
    const std::unique_ptr<GTexture> mFramebufferTexture = nullptr;

    const GLuint mFramebuffer = GL_NONE;
};

}  // namespace galena
