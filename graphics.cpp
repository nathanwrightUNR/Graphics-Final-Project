#include "graphics.h"

Graphics::Graphics() {}

Graphics::~Graphics() {}

bool Graphics::Initialize(int width, int height)
{
// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
  // cout << glewGetString(GLEW_VERSION) << endl;
  glewExperimental = GL_TRUE;

  auto status = glewInit();

  // This is here to grab the error that comes from glew init.
  // This error is an GL_INVALID_ENUM that has no effects on the performance
  glGetError();

  // Check for error
  if (status != GLEW_OK)
  {
    std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
    return false;
  }
#endif

  // instantiate Camera and assign pointer.
  m_camera = new Camera();
  if (!m_camera->Initialize(width, height))
  {
    printf("Camera Failed to Initialize\n");
    return false;
  }

  // instantiate Shader and assign pointer.
  m_shader = new Shader();

  if (!m_shader->Initialize())
  {
    printf("Shader Failed to Initialize\n");
    return false;
  }

  if (!m_shader->AddShader(GL_VERTEX_SHADER))
  {
    printf("Vertex Shader failed to Initialize\n");
    return false;
  }

  if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
  {
    printf("Fragment Shader failed to Initialize\n");
    return false;
  }

  if (!m_shader->Finalize())
  {
    printf("Program to Finalize\n");
    return false;
  }

  if (!collectShPrLocs())
  {
    printf("Some shader attribs not located!\n");
    return false;
  }

  m_starship = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f),
                        "../assets/SpaceShip-1/SpaceShip-1.obj",
                        "../assets/SpaceShip-1/SpaceShip-1.png");

  m_sun = new Sphere(64, "../assets/Planetary Textures/2k_sun.jpg");

  m_moon = new Sphere(32, "../assets/Planetary Textures/2k_moon.jpg");

  m_planet = new Sphere(48, "../assets/Planetary Textures/2k_earth_daymap.jpg");

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

Camera *Graphics::getCamera() { return this->m_camera; }

void Graphics::HierarchicalUpdate2(double dt)
{
  std::vector<float> speed, dist, rotSpeed, scale;
  glm::vec3 rotVector;
  glm::mat4 localTransform;

  // position of the sun
  this->modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));
  localTransform = this->modelStack.top();

  if (m_sun != NULL)
    m_sun->Update(localTransform);

  // position of the starship
  dist = {3.25, 0, 0};
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(dist[0], dist[1], dist[2]));

  localTransform *= glm::scale(glm::vec3(0.01f));
  this->modelStack.push(localTransform);

  if (m_starship != NULL)
    m_starship->Update(localTransform);

  this->modelStack.pop();

  // position of the planet
  dist = {6., 0, 0};
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(dist[0], dist[1], dist[2]));
  this->modelStack.push(localTransform);

  if (m_planet != NULL)
    m_planet->Update(localTransform);

  // position of the moon
  dist = {5., 0, 0};
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(dist[0], dist[1], dist[2]));
  this->modelStack.push(localTransform);
  if (m_moon != NULL)
    m_moon->Update(localTransform);

  this->modelStack.pop();

  this->modelStack.pop();

  this->modelStack.pop();
}

void Graphics::Render()
{
  // clear the screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Start the correct program
  m_shader->Enable();

  // Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

  if (m_sun != NULL)
  {
    glUniform1i(m_hasTexture, false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));
    if (m_sun->hasTex)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
      m_sun->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
    }
  }

  if (m_starship != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_starship->GetModel()));
    if (m_starship->hasTex)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_starship->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
      m_starship->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
    }
  }

  if (m_planet != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_planet->GetModel()));
    if (m_planet->hasTex)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_planet->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
      m_planet->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
    }
  }

  // Render Moon
  if (m_moon != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));
    if (m_moon->hasTex)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
      m_moon->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
    }
  }

  // Get any errors from OpenGL
  auto error = glGetError();
  if (error != GL_NO_ERROR)
  {
    string val = ErrorString(error);
    std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
  }
}

bool Graphics::collectShPrLocs()
{
  bool anyProblem = true;
  // Locate the projection matrix in the shader
  m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
  if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("m_projectionMatrix not found\n");
    anyProblem = false;
  }

  // Locate the view matrix in the shader
  m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
  if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("m_viewMatrix not found\n");
    anyProblem = false;
  }

  // Locate the model matrix in the shader
  m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
  if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("m_modelMatrix not found\n");
    anyProblem = false;
  }

  // Locate the position vertex attribute
  m_positionAttrib = m_shader->GetAttribLocation("v_position");
  if (m_positionAttrib == -1)
  {
    printf("v_position attribute not found\n");
    anyProblem = false;
  }

  // Locate the color vertex attribute
  m_colorAttrib = m_shader->GetAttribLocation("v_color");
  if (m_colorAttrib == -1)
  {
    printf("v_color attribute not found\n");
    anyProblem = false;
  }

  // Locate the texture coordinate attribute
  m_tcAttrib = m_shader->GetAttribLocation("v_tc");
  if (m_tcAttrib == -1)
  {
    printf("v_texcoord attribute not found\n");
    anyProblem = false;
  }

  // Locate the hasTexture uniform
  m_hasTexture = m_shader->GetUniformLocation("hasTexture");
  if (m_hasTexture == INVALID_UNIFORM_LOCATION)
  {
    printf("hasTexture uniform not found\n");
    anyProblem = false;
  }

  return anyProblem;
}

std::string Graphics::ErrorString(GLenum error)
{
  if (error == GL_INVALID_ENUM)
  {
    return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
  }

  else if (error == GL_INVALID_VALUE)
  {
    return "GL_INVALID_VALUE: A numeric argument is out of range.";
  }

  else if (error == GL_INVALID_OPERATION)
  {
    return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
  }

  else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
  {
    return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
  }

  else if (error == GL_OUT_OF_MEMORY)
  {
    return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
  }
  else
  {
    return "None";
  }
}
