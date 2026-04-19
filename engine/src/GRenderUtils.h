#pragma once

#include "GFramebuffer.h"

#include <GLES3/gl3.h>

namespace galena {

template <typename F>
void BindFramebufferOrPresent(GFramebuffer *pFramebuffer, uint32_t renderWidth,
    uint32_t renderHeight, F &&scope)
{
    if (!pFramebuffer)
    {
        glViewport(0, 0, renderWidth, renderHeight);

        scope();
        return;
    }

    pFramebuffer->Bind(std::forward<F>(scope));
}

}  // namespace galena
