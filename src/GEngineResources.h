#pragma once

#include "GEngineData.h"
#include "GHwBuffer.h"

#include <GLES3/gl3.h>

#include <iostream>
#include <memory>
#include <unordered_map>

namespace galena {

/* Distinction between engine data and engine resource:

   - Engine data is immutable pure data
   - Engine resources is created during runtime
*/

class GEngineResources
{
public:
    GEngineResources(GEngineData &&engineData,
        std::unordered_map<EGShaderKey, GLuint> shaders,
        std::unique_ptr<GHwBuffer> triangleBuffer,
        std::unique_ptr<GHwBuffer> quadBuffer)
        : mEngineData(std::move(engineData)), mShaders(std::move(shaders)),
          mTriangleBuffer(std::move(triangleBuffer)),
          mQuadBuffer(std::move(quadBuffer))
    {
    }

    GEngineResources(const GEngineResources &) = delete;

    GEngineResources(GEngineResources &&) = delete;

    ~GEngineResources();

public:
    static std::unique_ptr<GEngineResources> Create();

    template <typename F>
    void UseProgram(EGShaderKey shaderKey, F &&scope) const
    {
        const GLuint program = mShaders.at(shaderKey);

        glUseProgram(program);
        {
            scope();
        }
        glUseProgram(GL_NONE);
    }

    template <typename F> bool CatchErrors(F &&scope) const
    {
        scope();

        GLenum err = glGetError();

        if (err == GL_NO_ERROR)
            return true;

        std::cout << "GL error: " << err << std::endl;
        
        return false;
    }
    
    GHwBuffer *TriangleBuffer() const { return mTriangleBuffer.get(); }

    GHwBuffer *QuadBuffer() const { return mQuadBuffer.get(); }

public:
    const GEngineData mEngineData;

    const std::unordered_map<EGShaderKey, GLuint> mShaders;

    const std::unique_ptr<GHwBuffer> mTriangleBuffer;
    const std::unique_ptr<GHwBuffer> mQuadBuffer;
};

}  // namespace galena
