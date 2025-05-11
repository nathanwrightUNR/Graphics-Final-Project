#include "skybox.h"

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
    for (std::vector<GLuint>::iterator shader = m_shaderObjList.begin(); shader != m_shaderObjList.end(); shader++)
    {
        glDeleteShader(*shader);
    }

    if (m_shaderProg != 0)
    {
        glDeleteProgram(m_shaderProg);
        m_shaderProg = 0;
    }
}

bool Skybox::Initialize()
{
    m_shaderProg = glCreateProgram();

    if (m_shaderProg == 0)
    {
        std::cerr << "Error creating shader program\n";
        return false;
    }

    return true;
}

bool Skybox::AddShader(GLenum ShaderType)
{
    std::string s;

    if (ShaderType == GL_VERTEX_SHADER)
    {
        s = "#version 410\n\
            layout (location = 0) in vec3 position; \
            out vec3 texCoord; \
            uniform mat4 proj_matrix; \
            uniform mat4 v_matrix; \
            void main() \
            { \
                vec4 pos = proj_matrix * mat4(mat3(v_matrix)) * vec4(position, 1.0); \
                gl_Position = pos.xyww; \
                texCoord = position; \
            }";
    }
    else if (ShaderType == GL_FRAGMENT_SHADER)
    {
        // allows for setting emissiveness of skybox
        s = "#version 410\n\
            in vec3 texCoord; \
            out vec4 frag_color; \
            uniform samplerCube samp; \
            uniform bool is_emissive; \
            void main() \
            { \
                vec4 tex = texture(samp, texCoord); \
                frag_color = is_emissive ? tex * 1.03 : tex; \
            }";
    }
    else
    {
        std::cerr << "Unsupported shader type: " << ShaderType << std::endl;
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);
    if (ShaderObj == 0)
    {
        std::cerr << "Error creating shader type " << ShaderType << std::endl;
        return false;
    }

    m_shaderObjList.push_back(ShaderObj);

    const GLchar *source = s.c_str();
    GLint length = static_cast<GLint>(s.size());
    glShaderSource(ShaderObj, 1, &source, &length);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        std::cerr << "Error compiling: " << InfoLog << std::endl;
        return false;
    }

    glAttachShader(m_shaderProg, ShaderObj);

    return true;
}

bool Skybox::Finalize()
{
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(m_shaderProg);

    glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
        return false;
    }

#if !defined(__APPLE__) && !defined(MACOSX)
    glValidateProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
    if (!Success)
    {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
        return false;
    }
#endif
    for (std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
    {
        glDeleteShader(*it);
    }

    m_shaderObjList.clear();

    return true;
}

GLint Skybox::GetUniformLocation(const char *pUniformName)
{
    GLuint Location = glGetUniformLocation(m_shaderProg, pUniformName);

    if (Location == INVALID_UNIFORM_LOCATION)
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint Skybox::GetAttribLocation(const char *pAttribName)
{
    GLuint Location = glGetAttribLocation(m_shaderProg, pAttribName);

    if (Location == -1)
    {
        fprintf(stderr, "Warning! Unable to get the location of attribute '%s'\n", pAttribName);
    }

    return Location;
}
