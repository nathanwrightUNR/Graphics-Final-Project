#include "shader.h"

Shader::Shader()
{
  m_shaderProg = 0;
}

Shader::~Shader()
{
  for (std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
  {
    glDeleteShader(*it);
  }

  if (m_shaderProg != 0)
  {
    glDeleteProgram(m_shaderProg);
    m_shaderProg = 0;
  }
}

bool Shader::Initialize()
{
  m_shaderProg = glCreateProgram();

  if (m_shaderProg == 0)
  {
    std::cerr << "Error creating shader program\n";
    return false;
  }

  return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Shader::AddShader(GLenum ShaderType)
{
  std::string s;

  if (ShaderType == GL_VERTEX_SHADER)
  {
    s = "#version 410\n \
          \
          layout (location = 0) in vec3 v_position; \
          layout (location = 1) in vec2 v_tc; \
          layout (location = 2) in vec3 v_normal; \
          layout (location = 4) in mat4 imodelMatrix; \
          \
          out vec2 tc; \
          out vec3 varNorm; \
          out vec3 frag_pos; \
          \
          uniform mat4 projectionMatrix; \
          uniform mat4 viewMatrix; \
          uniform mat4 modelMatrix; \
          uniform mat3 normMatrix; \
          uniform bool instanced; \
          \
          void main(void) \
          { \
            vec4 v = vec4(v_position, 1.0); \
            mat4 model = instanced ? imodelMatrix : modelMatrix; \
            gl_Position = projectionMatrix * viewMatrix * model * v; \
            tc = v_tc; \
            varNorm = normMatrix * v_normal; \
            frag_pos = vec3(model * v); \
          }";
  }

  else if (ShaderType == GL_FRAGMENT_SHADER)
  {
    s = "#version 410\n \
          \
          struct material { \
            vec3 ambient; \
            vec3 diffuse; \
            vec3 specular; \
            float shininess; \
          }; \
          \
          in vec2 tc; \
          in vec3 varNorm; \
          in vec3 frag_pos; \
          \
          uniform bool is_emissive; \
          uniform bool hasNormalMap; \
          uniform bool hasTexture; \
          uniform sampler2D sp; \
          uniform sampler2D samp1; \
          uniform vec3 light_pos; \
          uniform vec3 light_color; \
          uniform vec3 view_pos; \
          uniform material mat; \
          \
          out vec4 frag_color; \
          \
          void main(void) \
          { \
            vec4 base_color = hasTexture ? texture(sp, tc) : vec4(1.0); \
            \
            if (is_emissive) { \
              frag_color = base_color; \
            } else { \
              vec3 norm = normalize(varNorm); \
              if (hasNormalMap) { \
                norm = normalize(norm + texture(samp1, tc).xyz * 2.0 - 1.0); \
              } \
              vec3 light_dir = normalize(light_pos - frag_pos); \
              float diff = max(dot(norm, light_dir), 0.0); \
              vec3 diffuse = diff * mat.diffuse * light_color; \
              \
              vec3 view_dir = normalize(view_pos - frag_pos); \
              vec3 reflect_dir = reflect(-light_dir, norm); \
              float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess); \
              vec3 specular = mat.specular * spec * light_color; \
              \
              vec3 ambient = mat.ambient * light_color; \
              vec3 result = (ambient + diffuse + specular) * vec3(base_color); \
              frag_color = vec4(result, 1.0); \
            } \
          }";
  }

  GLuint ShaderObj = glCreateShader(ShaderType);

  if (ShaderObj == 0)
  {
    std::cerr << "Error creating shader type " << ShaderType << std::endl;
    return false;
  }

  // Save the shader object - will be deleted in the destructor
  m_shaderObjList.push_back(ShaderObj);

  const GLchar *p[1];
  p[0] = s.c_str();
  GLint Lengths[1] = {(GLint)s.size()};

  glShaderSource(ShaderObj, 1, p, Lengths);

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

// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Shader::Finalize()
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

  // Delete the intermediate shader objects that have been added to the program
  for (std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
  {
    glDeleteShader(*it);
  }

  m_shaderObjList.clear();

  return true;
}

void Shader::Enable()
{
  glUseProgram(m_shaderProg);
}

GLint Shader::GetUniformLocation(const char *pUniformName)
{
  GLuint Location = glGetUniformLocation(m_shaderProg, pUniformName);

  if (Location == INVALID_UNIFORM_LOCATION)
  {
    fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
  }

  return Location;
}

GLint Shader::GetAttribLocation(const char *pAttribName)
{
  GLuint Location = glGetAttribLocation(m_shaderProg, pAttribName);

  if (Location == -1)
  {
    fprintf(stderr, "Warning! Unable to get the location of attribute '%s'\n", pAttribName);
  }

  return Location;
}
