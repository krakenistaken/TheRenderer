#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool loadFromFile(const std::string& vertPath, const std::string& fragPath);
    bool loadFromSource(const char* vertSrc, const char* fragSrc);
    void use() const;

    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    GLuint getProgram() const { return program; }
    void cleanup();

private:
    GLuint program = 0;
    mutable std::unordered_map<std::string, GLint> uniformCache;
    GLint getUniformLocation(const std::string& name) const;
    GLuint compileShader(GLenum type, const char* source);
};
