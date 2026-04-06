#include "GShader.h"

#include <GLES3/gl3.h>

#include <unordered_map>
#include <iostream>
#include <memory>
#include <optional>

namespace galena {

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

std::unique_ptr<GShader> GShader::Create(const std::string &vertexShaderSource,
    const std::string &fragmentShaderSource,
    const std::vector<std::string> &uniforms)
{
    std::optional<GLuint> vertexShader =
        CompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());

    if (!vertexShader)
        return nullptr;

    std::optional<GLuint> fragmentShader =
        CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    if (!fragmentShader)
        return nullptr;

    std::optional<GLuint> program = LinkProgram(*vertexShader, *fragmentShader);

    glDeleteShader(*vertexShader);
    glDeleteShader(*fragmentShader);

    if (!program)
        return nullptr;

    std::unordered_map<std::string, GLuint> uniformMap;
    
    for (const std::string &uniform : uniforms)
    {
        GLint location = glGetUniformLocation(*program, uniform.c_str());

        if (location < 0)
        {
            std::cerr << "Uniform location not found: " << uniform.c_str()
                      << std::endl;
        }

        uniformMap[uniform] = static_cast<GLuint>(location);
    }

    return std::make_unique<GShader>(*program, std::move(uniformMap));
}

}  // namespace galena
