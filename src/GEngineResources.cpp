#include "GEngineResources.h"
#include "GEngineData.h"
#include "GHwBuffer.h"

#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include <GLES3/gl3.h>

namespace galena {

GEngineResources::~GEngineResources()
{
    for (std::pair<EGShaderKey, GLuint> it : mShaders)
    {
        glDeleteProgram(it.second);
    }
}

std::optional<GLuint> CompileShader(GLuint shaderType, const char *czSource)
{
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &czSource, nullptr);
    glCompileShader(shader);

    GLint params = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);

    if (params != GL_TRUE)
    {
        char infoLog[512] = {0};

        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Failed to compile shader: " << infoLog << std::endl;

        glDeleteShader(shader);

        return std::nullopt;
    }

    return shader;
}

std::optional<GLuint> LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint params = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &params);

    if (params != GL_TRUE)
    {
        char infoLog[512] = {0};

        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Failed to link program: " << infoLog << std::endl;

        glDeleteProgram(program);

        return std::nullopt;
    }

    return program;
}

std::unique_ptr<GEngineResources> GEngineResources::Create()
{
    GEngineData data = GetEngineData();

    std::unordered_map<EGShaderKey, GLuint> shaders;

    for (std::pair<EGShaderKey, const GShaderData &> it : data.shaderData)
    {
        std::optional<GLuint> vertexShader = CompileShader(
            GL_VERTEX_SHADER, it.second.vertexShaderSource.c_str());

        if (!vertexShader)
            return nullptr;

        std::optional<GLuint> fragmentShader = CompileShader(
            GL_FRAGMENT_SHADER, it.second.fragmentShaderSource.c_str());

        if (!fragmentShader)
            return nullptr;

        std::optional<GLuint> program =
            LinkProgram(*vertexShader, *fragmentShader);

        glDeleteShader(*vertexShader);
        glDeleteShader(*fragmentShader);

        if (!program)
            return nullptr;

        shaders[it.first] = *program;
    }

    const float triangleData[] = {
        // clang-format off
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
        // clang-format on
    };

    GVertexBufferAttribs attrs(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    std::vector<GVertexBufferAttribs> attribs = {attrs};

    std::unique_ptr<GHwBuffer> triangleBuffer =
        GHwBuffer::Create(triangleData, sizeof(triangleData), attribs);

    const float quadData[] = {
        // clang-format off
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f
        // clang-format on
    };

    std::unique_ptr<GHwBuffer> quadBuffer =
        GHwBuffer::Create(quadData, sizeof(quadData), attribs);

    return std::make_unique<GEngineResources>(std::move(data),
        std::move(shaders), std::move(triangleBuffer), std::move(quadBuffer));
}

}  // namespace galena
