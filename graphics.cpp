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

  //Skybox
  m_skybox = new Skybox();
  if (!m_skybox->Initialize()) {
      printf("Skybox shader failed to initialize\n");
      return false;
  }

  if (!m_skybox->AddShader(GL_VERTEX_SHADER)) {
      printf("Skybox vertex shader failed to initialize\n");
      return false;
  }

  if (!m_skybox->AddShader(GL_FRAGMENT_SHADER)) {
      printf("Skybox fragment shader failed to initialize\n");
      return false;
  }

  if (!m_skybox->Finalize()) {
      printf("Skybox shader failed to finalize\n");
      return false;
  }

  //populate location binding
  if (!collectShPrLocs())
  {
    printf("Some shader attribs not located!\n");
    return false;
  }

  //skybox
  sbCubeMap = skyboxCubeMap();

  //setup verticies and values for skybox
  skyboxVertices = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
  };

  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);

  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * skyboxVertices.size(), &skyboxVertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  //endof skybox rendering code


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
  this->modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0))); // sun's coordinate
  localTransform = this->modelStack.top();                                   // The sun origin
  localTransform *= glm::rotate(glm::mat4(1.0f), (float)dt, glm::vec3(0.f, 1.f, 0.f));
  localTransform *= glm::scale(glm::vec3(.75, .75, .75));
  if (m_sun != NULL)
    m_sun->Update(localTransform);

  // position of the starship
  speed = {0, 1, 1};
  dist = {0, 3.25, 3.25};
  rotVector = {0., 0., 0.};
  rotSpeed = {.0, .0, .0};
  scale = {.02f, .02f, .02f};
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(0.f,
                                             cos(speed[1] * dt) * dist[1],
                                             sin(speed[2] * dt) * dist[2]));
  this->modelStack.push(localTransform);

  glm::vec3 starship_pos = glm::vec3(localTransform[3]);
  glm::vec3 sun_pos = glm::vec3(0.f, 0.f, 0.f);
  glm::vec3 to_sun = sun_pos - starship_pos;
  float angle = glm::atan(to_sun.y, to_sun.z);
  localTransform *= glm::rotate(glm::mat4(1.f), -angle, glm::vec3(1.f, 0.f, 0.f));
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

  if (m_starship != NULL)
    m_starship->Update(localTransform);

  this->modelStack.pop();

  // position of the planet
  speed = {1., 0., 1.};
  dist = {6., 0, 6.};
  rotVector = {1., 1., 1.};
  rotSpeed = {1., 1., 1.};
  scale = {.5, .5, .5};
  localTransform = this->modelStack.top();
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(cos(speed[0] * dt) * dist[0],
                                             0.f,
                                             sin(speed[2] * dt) * dist[2]));
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  if (m_planet != NULL)
    m_planet->Update(localTransform);

  // position of the moon
  speed = {1., 0., 1.};
  dist = {1.25, 0, 1.25};
  rotVector = {1., 1., 1.};
  rotSpeed = {1., 1., 1.};
  scale = {.2, .2, .2};
  localTransform = this->modelStack.top();
  localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(35.f), glm::vec3(1.f, 0.f, 0.f));
  localTransform *= glm::translate(glm::mat4(1.f),
                                   glm::vec3(cos(speed[0] * dt) * dist[0],
                                             0.f,
                                             sin(speed[2] * dt) * dist[2]));
  this->modelStack.push(localTransform);
  localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
  localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
  if (m_moon != NULL)
    m_moon->Update(localTransform);

  this->modelStack.pop(); // back to the sun coordinate

  this->modelStack.pop(); // pop planet

  this->modelStack.pop(); // empy stack
}

void Graphics::Render()
{
  // clear the screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //skybox
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
  glVertexAttribPointer(m_skyboxPositionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, sbCubeMap);
  glEnable(GL_CULL_FACE);
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

  //skybox
  m_skyboxProjMatrix = m_skybox->GetUniformLocation("proj_matrix");
  if (m_skyboxProjMatrix == INVALID_UNIFORM_LOCATION) {
      printf("skybox projmatrix not found\n");
      anyProblem = false;
  }

  m_skyboxViewMatrix = m_skybox->GetUniformLocation("v_matrix");
  if (m_skyboxViewMatrix == INVALID_UNIFORM_LOCATION) {
      printf("skybox vmatrix not found\n");
      anyProblem = false;
  }

  m_skyboxPositionAttrib = m_skybox->GetAttribLocation("position");
  if (m_skyboxPositionAttrib == INVALID_UNIFORM_LOCATION) {
      printf("skybox position not found\n");
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

GLuint Graphics::skyboxCubeMap() {
    GLuint textureID;
    int w, h, c;

    std::string horizontalCrossPath = "../assets/Cubemaps/Galaxy-cubemap2.png";

    // Load the horizontal cross image using SOIL
    unsigned char* data = SOIL_load_image(horizontalCrossPath.c_str(), &w, &h, &c, SOIL_LOAD_AUTO);
    if (!data) {
        std::cerr << "Failed to load horizontal cross cubemap image: " << horizontalCrossPath << std::endl;
        return 0;
    }

    // Calculate the size of each face (assuming a 4x3 layout)
    int faceWidth = w / 4;
    int faceHeight = h / 3;

    if (faceWidth != faceHeight) {
        std::cerr << "Error: Cubemap faces are not square!" << std::endl;
        SOIL_free_image_data(data);
        return 0;
    }

    // Create the cubemap texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Define the offsets for each face in the horizontal cross layout
    struct FaceOffset {
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
    for (int i = 0; i < 6; ++i) {
        int xOffset = faceOffsets[i].x * faceWidth;
        int yOffset = faceOffsets[i].y * faceHeight;

        // Extract the face data
        unsigned char* faceData = new unsigned char[faceWidth * faceHeight * c];
        for (int row = 0; row < faceHeight; ++row) {
            memcpy(
                faceData + row * faceWidth * c,
                data + ((yOffset + row) * w + xOffset) * c,
                faceWidth * c
            );
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
            faceData
        );

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
