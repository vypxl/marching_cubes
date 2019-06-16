#ifndef __shader_hpp_
#define __shader_hpp_

#include <string>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
    GLuint vert;
    GLuint geom;
    GLuint frag;

    std::map<std::string, GLuint> locations;

    std::string readFile(const std::string &filePath);
    int compileShader(GLuint id, const std::string &src);

public:
    GLuint getUniformLocation(const std::string &name);
    GLuint getAttribLocation(const std::string &name);
    ~Shader();

    GLuint prog;

    int loadFromFile(const std::string &vertexShaderFilename, const std::string &geometryShaderFilename, const std::string &fragmenShaderFilename);
    void bind();
    void unbind();

    inline void setUniform(const std::string &name, GLint x) { glUniform1i(getUniformLocation(name), x); }
    inline void setUniform(const std::string &name, GLfloat x) { glUniform1f(getUniformLocation(name), x); }
    inline void setUniform(const std::string &name, GLuint x) { glUniform1ui(getUniformLocation(name), x); }
    inline void setUniform(const std::string &name, bool x) { glUniform1i(getUniformLocation(name), x); }

    inline void setUniform(const std::string &name, const glm::vec2 &v) { glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(v)); }
    inline void setUniform(const std::string &name, const glm::vec3 &v) { glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(v)); }
    inline void setUniform(const std::string &name, const glm::vec4 &v) { glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(v)); }

    inline void setUniform(const std::string &name, const glm::mat3 &m) { glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)); }
    inline void setUniform(const std::string &name, const glm::mat4 &m) { glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)); }
};

#endif
