#include "GHwBuffer.h"

#include <GLES3/gl3.h>

#include <memory>

namespace galena {

std::unique_ptr<GHwBuffer> GHwBuffer::Create(const void *pData, size_t size,
    const std::vector<GVertexBufferAttribs> &attribs)
{
    if (!pData)
        return nullptr;

    GLuint vao = GL_NONE;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    GLuint vbo = GL_NONE;
    glGenBuffers(1, &vbo);

    if (vbo == GL_NONE)
        return nullptr;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, pData, GL_STATIC_DRAW);

    for (const GVertexBufferAttribs &attrs : attribs)
    {
        glVertexAttribPointer(attrs.index, attrs.size, attrs.type,
            attrs.normalized, attrs.stride, attrs.pOffset);

        glEnableVertexAttribArray(attrs.index);
    }

    return std::make_unique<GHwBuffer>(vao, vbo);
}

}  // namespace galena
