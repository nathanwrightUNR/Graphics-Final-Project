#ifndef SKYBOX_H
#define SKYBOX_H

#include "shader.h"

class Skybox : public Shader {
public:
    Skybox();
    ~Skybox();
    bool Initialize();
    void Enable() { glUseProgram(m_shaderProg); }
    bool AddShader(GLenum ShaderType);
    bool Finalize();
    GLint GetUniformLocation(const char* pUniformName);
    GLint GetAttribLocation(const char* pAttribName);
    GLuint GetShaderProgram() { return m_shaderProg; }

private:
    GLuint m_shaderProg = 0;
    std::vector<GLuint> m_shaderObjList;
};

#endif //SKYBOX_H