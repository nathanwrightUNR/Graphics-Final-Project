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

  // Skybox
  m_skybox = new Skybox();
  if (!m_skybox->Initialize())
  {
    printf("Skybox shader failed to initialize\n");
    return false;
  }

  if (!m_skybox->AddShader(GL_VERTEX_SHADER))
  {
    printf("Skybox vertex shader failed to initialize\n");
    return false;
  }

  if (!m_skybox->AddShader(GL_FRAGMENT_SHADER))
  {
    printf("Skybox fragment shader failed to initialize\n");
    return false;
  }

  if (!m_skybox->Finalize())
  {
    printf("Skybox shader failed to finalize\n");
    return false;
  }

  // populate location binding
  if (!collectShPrLocs())
  {
    printf("Some shader attribs not located!\n");
    return false;
  }

  // skybox
  sbCubeMap = skyboxCubeMap();

  // setup verticies and values for skybox
  skyboxVertices = {
      -1.0f, 1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,

      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, 1.0f};

  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);

  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * skyboxVertices.size(), &skyboxVertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  // endof skybox rendering code

  m_starship = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f),
                        "../assets/SpaceShip-1/SpaceShip-1.obj",
                        "../assets/SpaceShip-1/SpaceShip-1.png");

  m_sun = new Sphere(64, "../assets/Planetary Textures/2k_sun.jpg");

  m_mercury = new Sphere(64, "../assets/Planetary Textures/Mercury.jpg",
                         "../assets/Planetary Textures/Mercury-n.jpg");

  m_mercury_trace = new Ring(64, 4.425, 4.575);

  m_venus = new Sphere(64, "../assets/Planetary Textures/Venus.jpg",
                       "../assets/Planetary Textures/Venus-n.jpg");

  m_venus_trace = new Ring(64, 2.925, 3.075);

  m_earth = new Sphere(64, "../assets/Planetary Textures/2k_earth_daymap.jpg",
                       "../assets/Planetary Textures/2k_earth_daymap-n.jpg");

  m_earth_trace = new Ring(64, 5.925, 6.075);

  m_mars = new Sphere(64, "../assets/Planetary Textures/Mars.jpg",
                      "../assets/Planetary Textures/Mars-n.jpg");

  m_mars_trace = new Ring(64, 7.925, 8.075);

  m_jupiter = new Sphere(64, "../assets/Planetary Textures/Jupiter.jpg",
                         "../assets/Planetary Textures/Jupiter-n.jpg");

  m_jupiter_trace = new Ring(64, 11.925, 12.075);

  m_saturn = new Sphere(64, "../assets/Planetary Textures/Saturn.jpg");

  m_saturn_trace = new Ring(64, 15.925, 16.075);

  m_saturn_ring = new Ring(128, 1.3, 2, "../assets/Planetary Textures/Saturn_ring.png");

  m_uranus = new Sphere(64, "../assets/Planetary Textures/Uranus.jpg",
                        "../assets/Planetary Textures/Uranus-n.jpg");

  m_uranus_trace = new Ring(64, 20.925, 21.075);

  m_neptune = new Sphere(64, "../assets/Planetary Textures/Neptune.jpg",
                         "../assets/Planetary Textures/Neptune-n.jpg");

  m_neptune_trace = new Ring(64, 22.925, 23.075);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

Camera *Graphics::getCamera() { return this->m_camera; }

void Graphics::Update(double dt)
{
  std::vector<float> speed, dist, rotSpeed, scale;
  glm::vec3 rotVector;

  float r, theta, x, z;
  glm::mat4 localTransform;

  // position of the sun
  r = 0.;
  theta = glm::radians(0.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));

  if (m_sun != NULL)
    m_sun->Update(localTransform);

  // position of starship
  r = 1.;
  theta = glm::radians(90.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));

  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(0.01));

  if (m_starship != NULL)
    m_starship->Update(localTransform);

  // position of mercury
  r = 3.;
  theta = glm::radians(345.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.4));

  if (m_mercury != NULL)
    m_mercury->Update(localTransform);

  if (m_mercury_trace != NULL)
    m_mercury_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position venus
  r = 4.5;
  theta = glm::radians(15.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.5));

  if (m_venus != NULL)
    m_venus->Update(localTransform);

  if (m_venus_trace != NULL)
    m_venus_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position of earth
  r = 6.;
  theta = glm::radians(225.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.5));

  if (m_earth != NULL)
    m_earth->Update(localTransform);

  if (m_earth_trace != NULL)
    m_earth_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position of mars
  r = 8.;
  theta = glm::radians(80.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.4));

  if (m_mars != NULL)
    m_mars->Update(localTransform);

  if (m_mars_trace != NULL)
    m_mars_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position of jupiter
  r = 12.;
  theta = glm::radians(300.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.9));

  if (m_jupiter != NULL)
    m_jupiter->Update(localTransform);

  if (m_jupiter_trace != NULL)
    m_jupiter_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position of saturn
  r = 16.;
  theta = glm::radians(0.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.8));

  if (m_saturn != NULL)
    m_saturn->Update(localTransform);

  if (m_saturn_trace != NULL)
    m_saturn_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position of saturn ring
  localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));

  if (m_saturn_ring != NULL)
    m_saturn_ring->Update(localTransform);

  // position of uranus
  r = 21.;
  theta = glm::radians(200.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.6));

  if (m_uranus != NULL)
    m_uranus->Update(localTransform);

  if (m_uranus_trace != NULL)
    m_uranus_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // position of neptune
  r = 23.;
  theta = glm::radians(250.);
  x = r * glm::cos(theta);
  z = r * glm::sin(theta);
  localTransform = glm::mat4(1.);
  localTransform *= glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
  localTransform *= glm::scale(glm::mat4(1.f), glm::vec3(.6));

  if (m_neptune != NULL)
    m_neptune->Update(localTransform);

  if (m_neptune_trace != NULL)
    m_neptune_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));
}

void Graphics::Render()
{
  // clear the screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // skybox
  m_skybox->Enable();
  glUniformMatrix4fv(m_skyboxProjMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
  glUniformMatrix4fv(m_skyboxViewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
  GLint skyboxSampler = m_skybox->GetUniformLocation("samp");
  if (skyboxSampler == INVALID_UNIFORM_LOCATION)
  {
    printf("Skybox sampler not found\n");
  }
  glUniform1i(skyboxSampler, 0);
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(skyboxVAO);
  glEnableVertexAttribArray(m_skyboxPositionAttrib);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glVertexAttribPointer(m_skyboxPositionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, sbCubeMap);
  glFrontFace(GL_CCW);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glDisableVertexAttribArray(m_skyboxPositionAttrib);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glUseProgram(0);

  // Start the correct program
  m_shader->Enable();

  // Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

  if (m_sun != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));

    if (m_sun->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_sun->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_sun->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_sun->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_starship != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_starship->GetModel()));

    if (m_starship->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_starship->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_starship->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_starship->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_starship->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_mercury != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury->GetModel()));

    if (m_mercury->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_mercury->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_mercury->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_mercury->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_mercury->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_mercury_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_mercury_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_venus != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus->GetModel()));

    if (m_venus->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_venus->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_venus->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_venus->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_venus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_venus_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_venus_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_earth != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));

    if (m_earth->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_earth->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_earth->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_earth->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_earth_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_earth_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_mars != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars->GetModel()));

    if (m_mars->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_mars->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_mars->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_mars->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_mars->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_mars_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_mars_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_jupiter != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter->GetModel()));

    if (m_jupiter->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_jupiter->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_jupiter->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_jupiter->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_jupiter->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_jupiter_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_jupiter_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_saturn != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn->GetModel()));

    if (m_saturn->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_saturn->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_saturn->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_saturn->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_saturn->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_saturn_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_saturn_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_saturn_ring != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_ring->GetModel()));

    if (m_saturn_ring->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_saturn_ring->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_saturn_ring->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_saturn_ring->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_saturn_ring->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_uranus != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus->GetModel()));

    if (m_uranus->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_uranus->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_uranus->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_uranus->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_uranus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_uranus_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_uranus_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_neptune != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune->GetModel()));

    if (m_neptune->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_neptune->getTextureID());
      GLuint sampler = m_shader->GetUniformLocation("sp");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 0);
    }
    else
    {
      glUniform1i(m_hasTexture, false);
    }
    if (m_neptune->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_neptune->getNormalMapID());
      GLuint sampler = m_shader->GetUniformLocation("samp1");
      if (sampler == INVALID_UNIFORM_LOCATION)
      {
        printf("Sampler Not found not found\n");
      }
      glUniform1i(sampler, 1);
    }
    else
    {
      glUniform1i(m_has_nmap, false);
    }

    m_neptune->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_neptune_trace != NULL)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune_trace->GetModel()));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_neptune_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
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

  // Locate the normal vertex attribute
  m_NpAttrib = m_shader->GetAttribLocation("v_normal");
  if (m_NpAttrib == -1)
  {
    printf("v_normal attribute not found\n");
    anyProblem = false;
  }

  // Locate the texture coordinate attribute
  m_tcAttrib = m_shader->GetAttribLocation("v_tc");
  if (m_tcAttrib == -1)
  {
    printf("v_tc attribute not found\n");
    anyProblem = false;
  }

  // Locate the hasTexture uniform
  m_hasTexture = m_shader->GetUniformLocation("hasTexture");
  if (m_hasTexture == INVALID_UNIFORM_LOCATION)
  {
    printf("hasTexture uniform not found\n");
    anyProblem = false;
  }

  // skybox
  m_skyboxProjMatrix = m_skybox->GetUniformLocation("proj_matrix");
  if (m_skyboxProjMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("skybox projmatrix not found\n");
    anyProblem = false;
  }

  m_skyboxViewMatrix = m_skybox->GetUniformLocation("v_matrix");
  if (m_skyboxViewMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("skybox vmatrix not found\n");
    anyProblem = false;
  }

  m_skyboxPositionAttrib = m_skybox->GetAttribLocation("position");
  if (m_skyboxPositionAttrib == INVALID_UNIFORM_LOCATION)
  {
    printf("skybox position not found\n");
    anyProblem = false;
  }

  m_has_nmap = m_shader->GetUniformLocation("hasNormalMap");
  if (m_has_nmap == INVALID_UNIFORM_LOCATION)
  {
    printf("hasNormalMap uniform not found\n");
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

GLuint Graphics::skyboxCubeMap()
{
  GLuint textureID;
  int w, h, c;

  std::string horizontalCrossPath = "../assets/Cubemaps/Galaxy-cubemap2.png";

  // Load the horizontal cross image using SOIL
  unsigned char *data = SOIL_load_image(horizontalCrossPath.c_str(), &w, &h, &c, SOIL_LOAD_AUTO);
  if (!data)
  {
    std::cerr << "Failed to load horizontal cross cubemap image: " << horizontalCrossPath << std::endl;
    return 0;
  }

  // Calculate the size of each face (assuming a 4x3 layout)
  int faceWidth = w / 4;
  int faceHeight = h / 3;

  if (faceWidth != faceHeight)
  {
    std::cerr << "Error: Cubemap faces are not square!" << std::endl;
    SOIL_free_image_data(data);
    return 0;
  }

  // Create the cubemap texture
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  // Define the offsets for each face in the horizontal cross layout
  struct FaceOffset
  {
    int x, y;
  };
  FaceOffset faceOffsets[6] = {
      {2, 1}, // +X (right)
      {0, 1}, // -X (left)
      {1, 0}, // +Y (top)
      {1, 2}, // -Y (bottom)
      {1, 1}, // +Z (front)
      {3, 1}  // -Z (back)
  };

  // Extract and upload each face
  for (int i = 0; i < 6; ++i)
  {
    int xOffset = faceOffsets[i].x * faceWidth;
    int yOffset = faceOffsets[i].y * faceHeight;

    // Extract the face data
    unsigned char *faceData = new unsigned char[faceWidth * faceHeight * c];
    for (int row = 0; row < faceHeight; ++row)
    {
      memcpy(
          faceData + row * faceWidth * c,
          data + ((yOffset + row) * w + xOffset) * c,
          faceWidth * c);
    }

    // Upload the face to the cubemap
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0,
        c == 4 ? GL_RGBA : GL_RGB,
        faceWidth,
        faceHeight,
        0,
        c == 4 ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        faceData);

    delete[] faceData;
  }

  // Free the original image data
  SOIL_free_image_data(data);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}
