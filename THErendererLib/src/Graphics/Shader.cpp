#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <cstdio>

Shader::~Shader() { cleanup(); }

Shader::Shader(Shader&& other) noexcept
    : program(other.program), uniformCache(std::move(other.uniformCache)) {
    other.program = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        cleanup();
        program = other.program;
        uniformCache = std::move(other.uniformCache);
        other.program = 0;
    }
    return *this;
}

void Shader::cleanup() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
    uniformCache.clear();
}

GLuint Shader::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        fprintf(stderr, "Shader compilation error:\n%s\n", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::loadFromSource(const char* vertSrc, const char* fragSrc) {
    cleanup();

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    if (!vert) return false;

    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!frag) { glDeleteShader(vert); return false; }

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        fprintf(stderr, "Shader link error:\n%s\n", log);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program != 0;
}

bool Shader::loadFromFile(const std::string& vertPath, const std::string& fragPath) {
    auto readFile = [](const std::string& path) -> std::string {
        std::ifstream file(path);
        if (!file.is_open()) {
            fprintf(stderr, "Failed to open shader: %s\n", path.c_str());
            return "";
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    };

    std::string vertSrc = readFile(vertPath);
    std::string fragSrc = readFile(fragPath);
    if (vertSrc.empty() || fragSrc.empty()) return false;

    return loadFromSource(vertSrc.c_str(), fragSrc.c_str());
}

void Shader::use() const { glUseProgram(program); }

GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) return it->second;
    GLint loc = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = loc;
    return loc;
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}
