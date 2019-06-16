#include <fstream>
#include <iostream>
#include <vector>

#include "shader.hpp"

std::string Shader::readFile(const std::string &filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

int Shader::compileShader(GLuint id, const std::string &src) {
    const char *csrc = src.c_str();
    glShaderSource(id, 1, &csrc, nullptr);
    glCompileShader(id);

    int result = 0;
    int logLength;

    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

    logLength = logLength > 1 ? logLength : 1;
    std::vector<char> error(logLength);
    glGetShaderInfoLog(id, logLength, nullptr, &error[0]);

    if (result == GL_FALSE) {
        std::cout << "shader compilation result: " << result << std::endl << " error log: " << &error[0] << std::endl;
    }

    return result;
}

int Shader::loadFromFile(const std::string &vertexShaderFilename, const std::string &geometryShaderFilename, const std::string &fragmenShaderFilename) {
    unbind();
    glDeleteProgram(prog);
    prog = 0;
    bool useGeom = geometryShaderFilename != "";

    vert = glCreateShader(GL_VERTEX_SHADER);
    if (useGeom) geom = glCreateShader(GL_GEOMETRY_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertSrc = readFile(vertexShaderFilename);
    std::string geomSrc;
    if (useGeom) geomSrc = readFile(geometryShaderFilename);
    std::string fragSrc = readFile(fragmenShaderFilename);

    compileShader(vert, vertSrc);
    if (useGeom) compileShader(geom, geomSrc);
    compileShader(frag, fragSrc);

    prog = glCreateProgram();
    glAttachShader(prog, vert);
    if (useGeom) glAttachShader(prog, geom);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    int result = 0;
    int logLength;
    glGetProgramiv(prog, GL_LINK_STATUS, &result);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);

    logLength = logLength > 1 ? logLength : 1;
    std::vector<char> error(logLength);
    glGetProgramInfoLog(prog, logLength, nullptr, &error[0]);

    if (result == GL_FALSE) {
        std::cout << "program linking result: " << result << std::endl << " error log: " << &error[0] << std::endl;
    }

    glDeleteShader(vert);
    if (useGeom) glDeleteShader(geom);
    glDeleteShader(frag);

    return result;
}

void Shader::bind() {
    glUseProgram(prog);
}

void Shader::unbind() {
    int cur;
    glGetIntegerv(GL_CURRENT_PROGRAM, &cur);
    if (cur == (int)prog) 
        glUseProgram(0);
}

GLuint Shader::getUniformLocation(const std::string &name) {
    std::map<std::string, GLuint>::iterator res;
    res = locations.find(name);
    if (res != locations.end()) return res->second;
    else {
      GLuint loc = glGetUniformLocation(prog, name.c_str());
      locations.insert(std::pair<std::string, GLuint>(name, loc));
      return loc;
    }
}

GLuint Shader::getAttribLocation(const std::string &name) {
  return glGetAttribLocation(prog, name.c_str());
}

/* inline void Shader::setUniform(const std::string &name, GLint x) { glUniform1i(getUniformLocation(name), x); } */
/* inline void Shader::setUniform(const std::string &name, GLfloat x) { glUniform1f(getUniformLocation(name), x); } */
/* inline void Shader::setUniform(const std::string &name, GLuint x) { glUniform1ui(getUniformLocation(name), x); } */
/* inline void Shader::setUniform(const std::string &name, bool x) { glUniform1i(getUniformLocation(name), x); } */

/* inline void Shader::setUniform(const std::string &name, const glm::vec2 &v) { glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(v)); } */
/* inline void Shader::setUniform(const std::string &name, const glm::vec3 &v) { glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(v)); } */
/* inline void Shader::setUniform(const std::string &name, const glm::vec4 &v) { glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(v)); } */

/* inline void Shader::setUniform(const std::string &name, const glm::mat3 &m) { glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)); } */
/* inline void Shader::setUniform(const std::string &name, const glm::mat4 &m) { glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)); } */

Shader::~Shader() {
    unbind();
    glDeleteProgram(prog);
}
