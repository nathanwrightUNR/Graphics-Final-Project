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

  generateAsteroidTransforms(m_inner_asteroid_belt1, m_inner_asteroid_angles1, 200, 14.f, 250);
  generateAsteroidTransforms(m_inner_asteroid_belt2, m_inner_asteroid_angles2, 200, 14.f, 250);
  generateAsteroidTransforms(m_inner_asteroid_belt3, m_inner_asteroid_angles3, 200, 14.f, 250);

  m_inner_asteroid1 = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                               "../assets/Asteroid/1132 T-3 Durech.obj",
                               "../assets/Planetary Textures/Haumea.jpg");

  m_inner_asteroid1->Instance(this->m_inner_asteroid_belt1);

  m_inner_asteroid2 = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                               "../assets/Asteroid/1978 XX Durech.obj",
                               "../assets/Planetary Textures/Eris.jpg");

  m_inner_asteroid2->Instance(this->m_inner_asteroid_belt2);

  m_inner_asteroid3 = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                               "../assets/Asteroid/1998 DQ3 Durech.obj",
                               "../assets/Planetary Textures/Ceres.jpg");

  generateAsteroidTransforms(m_outer_asteroid_belt1, m_outer_asteroid_angles1, 800, 41.5f, 1200);
  generateAsteroidTransforms(m_outer_asteroid_belt2, m_outer_asteroid_angles2, 800, 41.5f, 1200);
  generateAsteroidTransforms(m_outer_asteroid_belt3, m_outer_asteroid_angles3, 800, 41.5f, 1200);

  m_inner_asteroid3->Instance(this->m_inner_asteroid_belt3);

  m_outer_asteroid1 = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                               "../assets/Asteroid/1132 T-3 Durech.obj",
                               "../assets/Planetary Textures/Haumea.jpg");

  m_outer_asteroid1->Instance(this->m_outer_asteroid_belt1);

  m_outer_asteroid2 = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                               "../assets/Asteroid/1978 XX Durech.obj",
                               "../assets/Planetary Textures/Eris.jpg");

  m_outer_asteroid2->Instance(this->m_outer_asteroid_belt2);

  m_outer_asteroid3 = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                               "../assets/Asteroid/1998 DQ3 Durech.obj",
                               "../assets/Planetary Textures/Ceres.jpg");

  m_outer_asteroid3->Instance(this->m_outer_asteroid_belt3);

  m_halleys = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                       "../assets/Asteroid/Halley Giotto_Vega Stooke Model 1.obj");

  m_starship = new Mesh(glm::vec3(2.f, 3.f, -5.f),
                        "../assets/SpaceShip-1/SpaceShip-1.obj",
                        "../assets/SpaceShip-1/SpaceShip-1.png");

  m_sun = new Sphere(64, 0., "../assets/Planetary Textures/2k_sun.jpg");

  m_mercury = new Sphere(64, 90, "../assets/Planetary Textures/Mercury.jpg",
                         "../assets/Planetary Textures/Mercury-n.jpg");

  m_mercury_trace = new Ring(64, 4.45, 4.55);

  m_venus = new Sphere(64, 45, "../assets/Planetary Textures/Venus.jpg",
                       "../assets/Planetary Textures/Venus-n.jpg");

  m_venus_trace = new Ring(64, 6.45, 6.55);

  m_earth = new Sphere(64, 90, "../assets/Planetary Textures/2k_earth_daymap.jpg",
                       "../assets/Planetary Textures/2k_earth_daymap-n.jpg");

  m_moon = new Sphere(64, 0, "../assets/Planetary Textures/2k_moon.jpg",
                      "../assets/Planetary Textures/2k_moon-n.jpg");

  m_earth_trace = new Ring(64, 9.45, 9.55);

  m_mars = new Sphere(64, 135, "../assets/Planetary Textures/Mars.jpg",
                      "../assets/Planetary Textures/Mars-n.jpg");

  m_mars_trace = new Ring(64, 12.95, 13.05);

  m_jupiter = new Sphere(64, 180, "../assets/Planetary Textures/Jupiter.jpg",
                         "../assets/Planetary Textures/Jupiter-n.jpg");

  m_jupiter_trace = new Ring(64, 17.95, 18.05);

  m_saturn = new Sphere(64, 225, "../assets/Planetary Textures/Saturn.jpg");

  m_saturn_trace = new Ring(64, 23.45, 23.55);

  m_saturn_ring = new Ring(128, 1.3, 2, "../assets/Planetary Textures/Saturn_ring.png");

  m_uranus = new Sphere(64, 270, "../assets/Planetary Textures/Uranus.jpg",
                        "../assets/Planetary Textures/Uranus-n.jpg");

  m_uranus_trace = new Ring(64, 30.45, 30.55);

  m_neptune = new Sphere(64, 315, "../assets/Planetary Textures/Neptune.jpg",
                         "../assets/Planetary Textures/Neptune-n.jpg");

  m_neptune_trace = new Ring(64, 38.45, 38.55);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

Camera *Graphics::getCamera() { return this->m_camera; }

void Graphics::Update(double dt)
{
  std::vector<float> speed, dist, rotSpeed, scale;
  glm::vec3 rotVector, orbit_pos;

  float theta;
  glm::mat4 localTransform;

  // position of the sun
  this->modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));
  localTransform = this->modelStack.top();

  if (m_sun != NULL)
    m_sun->Update(localTransform);

  // animate mercury
  speed = {0.08, 0., 0.08};
  dist = {4.5, 0., 4.5};
  rotVector = {0., 1., 0.};
  rotSpeed = {.5, .5, .5};
  scale = {.4, .4, .4};

  theta = speed[0] * dt + m_mercury->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_mercury != NULL)
    m_mercury->Update(localTransform);

  if (m_mercury_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_mercury_trace->Update(trace_transform);
  }

  // animate venus
  speed = {0.06, 0., 0.06};
  dist = {6.5, 0., 6.5};
  rotSpeed = {.3, .3, .3};
  scale = {.5, .5, .5};

  theta = speed[0] * dt + m_venus->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_venus != NULL)
    m_venus->Update(localTransform);

  if (m_venus_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_venus_trace->Update(trace_transform);
  }

  // animate earth
  speed = {0.05, 0., 0.05};
  dist = {9.5, 0., 9.5};
  rotVector = {0., 1., 0.};
  rotSpeed = {.4, .4, .4};
  scale = {.5, .5, .5};

  theta = speed[0] * dt + m_earth->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

  if (m_earth != NULL)
    m_earth->Update(localTransform);

  if (m_earth_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_earth_trace->Update(trace_transform);
  }

  // animate moon
  speed = {.2, 0., .2};
  dist = {1.5, 0, 1.5};
  rotVector = {1., 1., 1.};
  rotSpeed = {1., 1., 1.};
  scale = {.1, .1, .1};

  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(cos(speed[0] * dt) * dist[0],
                                             0.f,
                                             sin(speed[2] * dt) * dist[2]));
  localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(35.f), glm::vec3(1.f, 0.f, 0.f));
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

  if (m_moon != NULL)
    m_moon->Update(localTransform);

  this->modelStack.pop();
  this->modelStack.pop();

  // animate mars
  speed = {0.04, 0., 0.04};
  dist = {13., 0., 13.};
  rotVector = {0., 1., 0.};
  rotSpeed = {0.5, 0.5, 0.5};
  scale = {.4, .4, .4};

  theta = speed[0] * dt + m_mars->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_mars != NULL)
    m_mars->Update(localTransform);

  if (m_mars_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_mars_trace->Update(trace_transform);
  }

  // animate jupiter
  speed = {0.025, 0., 0.025};
  dist = {18., 0., 18.};
  rotVector = {0., 1., 0.};
  rotSpeed = {0.3, 0.3, 0.3};
  scale = {.9, .9, .9};

  theta = speed[0] * dt + m_jupiter->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_jupiter != NULL)
    m_jupiter->Update(localTransform);

  if (m_jupiter_trace != NULL)
    m_jupiter_trace->Update(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0)));

  // animate saturn
  speed = {0.015, 0., 0.015};
  dist = {23.5, 0., 23.5};
  rotVector = {0., 1., 0.};
  rotSpeed = {0.2, 0.2, 0.2};
  scale = {.8, .8, .8};

  theta = speed[0] * dt + m_saturn->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_saturn != NULL)
    m_saturn->Update(localTransform);

  if (m_saturn_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_saturn_trace->Update(trace_transform);
  }

  // position of saturn ring
  localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));

  if (m_saturn_ring != NULL)
    m_saturn_ring->Update(localTransform);

  // animate uranus
  speed = {0.01, 0., 0.01};
  dist = {30.5, 0., 30.5};
  rotVector = {0., 1., 0.};
  rotSpeed = {0.3, 0.3, 0.3};
  scale = {.6, .6, .6};

  theta = speed[0] * dt + m_uranus->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_uranus != NULL)
    m_uranus->Update(localTransform);

  if (m_uranus_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_uranus_trace->Update(trace_transform);
  }

  // animate neptune
  speed = {0.0075, 0., 0.0075};
  dist = {38.5, 0., 38.5};
  rotVector = {0., 1., 0.};
  rotSpeed = {0.25, 0.25, 0.25};
  scale = {.6, .6, .6};

  theta = speed[0] * dt + m_neptune->angle;
  orbit_pos = glm::vec3(cos(theta) * dist[0], 0.f, sin(theta) * dist[2]);
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f), orbit_pos);
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  this->modelStack.pop();

  if (m_neptune != NULL)
    m_neptune->Update(localTransform);

  if (m_neptune_trace != NULL)
  {
    glm::mat4 trace_transform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.1f, 0.f));
    trace_transform *= glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    m_neptune_trace->Update(trace_transform);
  }

  updateAsteroidBelt(m_inner_asteroid_belt1, m_inner_asteroid1);
  updateAsteroidBelt(m_inner_asteroid_belt2, m_inner_asteroid2);
  updateAsteroidBelt(m_inner_asteroid_belt3, m_inner_asteroid3);

  updateAsteroidBelt(m_outer_asteroid_belt1, m_outer_asteroid1);
  updateAsteroidBelt(m_outer_asteroid_belt2, m_outer_asteroid2);
  updateAsteroidBelt(m_outer_asteroid_belt3, m_outer_asteroid3);
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

  glm::mat4 view = m_camera->GetView();
  glm::vec3 viewPos = glm::vec3(glm::inverse(view)[3]);

  glm::vec4 light = m_sun->GetModel() * glm::vec4(0, 0, 0, 1);
  glm::vec3 lightPos = glm::vec3(light);

  glUniform3f(m_shader->GetUniformLocation("view_pos"), viewPos.x, viewPos.y, viewPos.z);
  glUniform3f(m_shader->GetUniformLocation("light_pos"), lightPos.x, lightPos.y, lightPos.z);
  glUniform3f(m_shader->GetUniformLocation("light_color"), 1.0f, 1.0f, 1.0f);
  glUniform1i(m_shader->GetUniformLocation("is_emissive"), false);

  // Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(view));

  if (m_sun != NULL)
  {
    SetMaterialUniforms(glm::vec3(1.0f, 0.9f, 0.2f), // ambient
                        glm::vec3(1.0f, 0.8f, 0.1f), // diffuse
                        glm::vec3(1.0f, 1.0f, 0.5f), // specular
                        64.0f);                      // shininess

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_sun->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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

  if (m_starship != NULL && !oribing)
  {
    SetMaterialUniforms(glm::vec3(0.05f, 0.05f, 0.05f),
                        glm::vec3(0.6f, 0.6f, 0.6f),
                        glm::vec3(1.0f, 1.0f, 1.0f),
                        128.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_starship->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_starship->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_mercury->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_mercury_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_mercury_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_venus != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_venus->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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

    m_venus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_venus_trace != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_venus_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_venus_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_earth != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_earth->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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

  if (m_moon != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_moon->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

    if (m_moon->hasTex)
    {
      glUniform1i(m_hasTexture, true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID());
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
    if (m_moon->hasNmap)
    {
      glUniform1i(m_has_nmap, true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_moon->getNormalMapID());
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

    m_moon->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_earth_trace != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_earth_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_earth_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_mars != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_mars->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_mars_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_mars_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_jupiter != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_jupiter->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_jupiter_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_jupiter_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_saturn != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_saturn->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_saturn_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_saturn_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_saturn_ring != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.6f),
                        glm::vec3(0.8f),
                        glm::vec3(0.2f),
                        8.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_ring->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_saturn_ring->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_uranus->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_uranus_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_uranus_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_neptune != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.4f),
                        32.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_neptune->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

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
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniform1i(m_shader->GetUniformLocation("instanced"), false);
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune_trace->GetModel()));
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_neptune_trace->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, false);
    glUniform1i(m_has_nmap, false);

    m_neptune_trace->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_inner_asteroid1 != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_inner_asteroid1->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), true);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_inner_asteroid1->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

    glUniform1i(m_hasTexture, true);
    glUniform1i(m_has_nmap, false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inner_asteroid1->getTextureID());
    GLuint sampler = m_shader->GetUniformLocation("sp");
    if (sampler == INVALID_UNIFORM_LOCATION)
    {
      printf("Sampler Not found not found\n");
    }
    glUniform1i(sampler, 0);
    m_inner_asteroid1->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_inner_asteroid2 != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_inner_asteroid2->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), true);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_inner_asteroid2->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, true);
    glUniform1i(m_has_nmap, false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inner_asteroid2->getTextureID());
    GLuint sampler = m_shader->GetUniformLocation("sp");
    if (sampler == INVALID_UNIFORM_LOCATION)
    {
      printf("Sampler Not found not found\n");
    }
    glUniform1i(sampler, 0);
    m_inner_asteroid2->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_inner_asteroid3 != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_inner_asteroid3->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), true);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_inner_asteroid3->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, true);
    glUniform1i(m_has_nmap, false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inner_asteroid3->getTextureID());
    GLuint sampler = m_shader->GetUniformLocation("sp");
    if (sampler == INVALID_UNIFORM_LOCATION)
    {
      printf("Sampler Not found not found\n");
    }
    glUniform1i(sampler, 0);
    m_inner_asteroid3->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_outer_asteroid1 != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_outer_asteroid1->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), true);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_outer_asteroid1->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, true);
    glUniform1i(m_has_nmap, false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_outer_asteroid1->getTextureID());
    GLuint sampler = m_shader->GetUniformLocation("sp");
    if (sampler == INVALID_UNIFORM_LOCATION)
    {
      printf("Sampler Not found not found\n");
    }
    glUniform1i(sampler, 0);
    m_outer_asteroid1->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_outer_asteroid2 != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_outer_asteroid2->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), true);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_outer_asteroid2->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, true);
    glUniform1i(m_has_nmap, false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_outer_asteroid2->getTextureID());
    GLuint sampler = m_shader->GetUniformLocation("sp");
    if (sampler == INVALID_UNIFORM_LOCATION)
    {
      printf("Sampler Not found not found\n");
    }
    glUniform1i(sampler, 0);
    m_outer_asteroid2->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  if (m_outer_asteroid3 != NULL)
  {
    SetMaterialUniforms(glm::vec3(0.0f),
                        glm::vec3(1.0f),
                        glm::vec3(0.0f),
                        1.0f);

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_outer_asteroid3->GetModel()));
    glUniform1i(m_shader->GetUniformLocation("instanced"), true);
    glm::mat3 matrix = glm::transpose(glm::inverse(glm::mat3(m_outer_asteroid3->GetModel())));
    glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(m_hasTexture, true);
    glUniform1i(m_has_nmap, false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_outer_asteroid3->getTextureID());
    GLuint sampler = m_shader->GetUniformLocation("sp");
    if (sampler == INVALID_UNIFORM_LOCATION)
    {
      printf("Sampler Not found not found\n");
    }
    glUniform1i(sampler, 0);
    m_outer_asteroid3->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_NpAttrib, m_has_nmap);
  }

  // Get any errors from OpenGL
  auto error = glGetError();
  if (error != GL_NO_ERROR)
  {
    string val = ErrorString(error);
    std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
  }
}

void Graphics::SetMaterialUniforms(const glm::vec3 &ambient,
                                   const glm::vec3 &diffuse,
                                   const glm::vec3 &specular,
                                   float shininess)
{
  GLint loc;

  loc = m_shader->GetUniformLocation("mat.ambient");
  if (loc != INVALID_UNIFORM_LOCATION)
    glUniform3fv(loc, 1, glm::value_ptr(ambient));

  loc = m_shader->GetUniformLocation("mat.diffuse");
  if (loc != INVALID_UNIFORM_LOCATION)
    glUniform3fv(loc, 1, glm::value_ptr(diffuse));

  loc = m_shader->GetUniformLocation("mat.specular");
  if (loc != INVALID_UNIFORM_LOCATION)
    glUniform3fv(loc, 1, glm::value_ptr(specular));

  loc = m_shader->GetUniformLocation("mat.shininess");
  if (loc != INVALID_UNIFORM_LOCATION)
    glUniform1f(loc, shininess);
}

bool Graphics::collectShPrLocs()
{
  bool anyProblem = true;

  m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
  if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("m_projectionMatrix not found\n");
    anyProblem = false;
  }

  m_normalMatrix = m_shader->GetUniformLocation("normMatrix");
  if (m_normalMatrix == INVALID_UNIFORM_LOCATION)
  {
    printf("normMatrix uniform not found\n");
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
      {0, 1}, // -X (left)
      {2, 1}, // +X (right)
      {1, 2}, // -Y (bottom)
      {1, 0}, // +Y (top)
      {3, 1}, // -Z (back)
      {1, 1}, // +Z (front)
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

void Graphics::generateAsteroidTransforms(std::vector<glm::mat4> &belt,
                                          std::vector<float> &angles,
                                          int count,
                                          float radius,
                                          int span)
{
  belt.clear();
  angles.clear();

  for (int i = 0; i < count; i++)
  {
    float theta = glm::radians((float)(rand() % 360));
    float r = radius + (float)(rand() % span) / 100.f;

    float x = glm::cos(theta) * r;
    float z = glm::sin(theta) * r;
    float y = ((rand() % 100) - 50) * 0.01f;

    glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(x, y, z));
    transform = glm::rotate(transform, glm::radians((float)(rand() % 360)), glm::vec3(0.f, 1.f, 0.f));
    transform = glm::scale(transform, glm::vec3(0.1f));

    belt.push_back(transform);
    angles.push_back(theta);
  }
}

void Graphics::updateAsteroidBelt(std::vector<glm::mat4> &belt, Object *asteroid)
{
  for (auto &transform : belt)
  {
    glm::vec3 pos = glm::vec3(transform[3]);

    float r = glm::length(glm::vec2(pos.x, pos.z));
    float theta = glm::atan(pos.z, pos.x) + glm::radians(0.02f);

    float x = glm::cos(theta) * r;
    float z = glm::sin(theta) * r;
    float y = pos.y;

    glm::mat4 updated = glm::translate(glm::mat4(1.f), glm::vec3(x, y, z));
    updated = glm::scale(updated, glm::vec3(0.1f));

    transform = updated;
  }

  asteroid->UpdateInstanceBuffer(belt);
}

Object *Graphics::getStarship()
{
  return this->m_starship;
}

Object *Graphics::getClosestPlanet()
{
  glm::vec3 camera_pos = m_camera->GetPosition();
  float min = FLT_MAX;
  Object *closest_object;

  std::vector<Object *> objects = {
      m_sun, m_mercury, m_venus, m_earth, m_mars, m_jupiter,
      m_saturn, m_uranus, m_neptune};

  for (Object *object : objects)
  {
    if (!object)
      continue;

    glm::vec3 pos = glm::vec3(object->GetModel()[3]);
    float dist = glm::distance(camera_pos, pos);

    if (dist < min)
    {
      min = dist;
      closest_object = object;
    }
  }

  return closest_object;
}

void Graphics::SetOrbit(bool b)
{
  this->oribing = b;
}