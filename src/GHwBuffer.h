#pragma once

#include <GLES3/gl3.h>

#include <memory>

#include <iostream>
#include <vector>

namespace galena {

struct GVertexBufferAttribs
{
    size_t index = 0;
    size_t size = 0;
    GLuint type = GL_FLOAT;
    GLuint normalized = GL_FALSE;
    size_t stride = 0;
    const void *pOffset = nullptr;

public:
    GVertexBufferAttribs() = default;

    GVertexBufferAttribs(size_t index, size_t size, GLuint type,
        GLuint normalized, size_t stride, const void *pOffset)
        : index(index), size(size), type(type), normalized(normalized),
          stride(stride), pOffset(pOffset)
    {
    }
};

class GHwBuffer
{
public:
    GHwBuffer(GLuint vao, GLuint vbo)
        : mVertexArrayObject(vao), mVertexBufferObject(vbo)
    {
    }

    ~GHwBuffer()
    {
        if (mVertexArrayObject != GL_NONE)
        {
            glDeleteVertexArrays(1, &mVertexArrayObject);
        }

        if (mVertexBufferObject != GL_NONE)
        {
            glDeleteBuffers(1, &mVertexBufferObject);
        }
    }

public:
    static std::unique_ptr<GHwBuffer> Create(const void *pData, size_t size,
        const std::vector<GVertexBufferAttribs> &attribs);

public:
    template <typename F> void Bind(F &&scope)
    {
        glBindVertexArray(mVertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);

        {
            scope();
        }
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    }

private:
    const GLuint mVertexArrayObject = 0;
    const GLuint mVertexBufferObject = 0;
};

}  // namespace galena
