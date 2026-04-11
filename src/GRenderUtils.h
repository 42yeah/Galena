#pragma once

#include "GFramebuffer.h"

namespace galena {

template <typename F>
void BindFramebufferOrPresent(GFramebuffer *pFramebuffer, F &&scope)
{
    if (!pFramebuffer)
    {
        scope();
        return;
    }

    pFramebuffer->Bind(std::forward<F>(scope));
}

}  // namespace galena
