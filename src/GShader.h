#pragma once

#include <GLES3/gl3.h>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace galena {

class GShader
{
public:
    GShader(GLuint program, std::unordered_map<std::string, GLuint> locations)
        : mProgram(program), mLocations(locations)
    {
    }

public:
    static std::unique_ptr<GShader> Create(const std::string &vertexShaderSource,
        const std::string &fragmentShaderSource,
        const std::vector<std::string> &uniforms);

public:
    template <typename F> void Bind(F &&scope) const
    {
        glUseProgram(mProgram);
        {
            scope();
        }
        glUseProgram(GL_NONE);
    }

    GLuint Location(const std::string &uniformName) const
    {
        return mLocations.at(uniformName);
    }

private:
    const GLuint mProgram = GL_NONE;

    const std::unordered_map<std::string, GLuint> mLocations;
};

}  // namespace galena
