
#include "engine.h"

Engine::Engine(const char *name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;
}

Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if (!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if (!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  glfwSetScrollCallback(m_window->getWindow(), scroll_callback);
  glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetWindowUserPointer(m_window->getWindow(), this);
  glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);

  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
    double now = glfwGetTime();
    double dt = getDT();

    float speed = 1.0f;
    Object *starship = m_graphics->getStarship();
    Camera *camera = m_graphics->getCamera();

    ProcessInput();
    if (camera->forward)
      starship->Move(camera->GetFront(), speed, dt);

    m_graphics->getCamera()->Update(dt);

    if (first_person)
    {
      glm::vec3 planet_pos = target->GetModel()[3];
      float orbit_radius = 3.0f;
      float angle = glfwGetTime();

      glm::vec3 offset = glm::vec3(glm::cos(angle), 1.f, glm::sin(angle)) * orbit_radius;
      glm::vec3 orbit_pos = planet_pos + offset;

      camera->position = orbit_pos;
      camera->SetView(glm::lookAt(orbit_pos, planet_pos, glm::vec3(0, 1, 0)));
    }
    else
    {
      glm::vec3 camera_pos = camera->GetPosition();
      glm::vec3 front = camera->GetFront();
      glm::vec3 ship_pos = camera_pos - front * -2.0f;

      glm::mat4 rotation = glm::lookAt(glm::vec3(0.0f), front, glm::vec3(0, 1, 0));
      rotation = glm::inverse(rotation);
      rotation *= glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

      glm::mat4 model = glm::translate(glm::mat4(1.0f), ship_pos) * rotation;
      model = glm::scale(model, glm::vec3(0.01f));

      starship->Update(model);
    }

    Display(m_window->getWindow(), now);
    glfwPollEvents();
  }

  m_running = false;
}

void Engine::ProcessInput()
{
  static bool space_pressed_last_frame = false;

  if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(m_window->getWindow(), true);

  Camera *camera = this->m_graphics->getCamera();
  camera->forward = glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS;

  bool space_pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS;

  if (space_pressed && !space_pressed_last_frame)
  {
    first_person = !first_person;
    camera->SetFirstPerson(first_person);

    m_graphics->SetOrbit(first_person);

    if (first_person)
      target = m_graphics->getClosestPlanet();
  }

  space_pressed_last_frame = space_pressed;
}

float Engine::getDT()
{
  // long long TimeNowMillis = GetCurrentTimeMillis();
  // assert(TimeNowMillis >= m_currentTimeMillis);
  // unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  // m_currentTimeMillis = TimeNowMillis;
  // return DeltaTimeMillis;
  double now = glfwGetTime();
  float dt = now - this->last_time;
  this->last_time = now;
  return dt;
}

long long Engine::GetCurrentTimeMillis()
{
  // timeval t;
  // gettimeofday(&t, NULL);
  // long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  // return ret;
  return (long long)glfwGetTime();
}

void Engine::Display(GLFWwindow *window, double time)
{
  m_graphics->Render();
  m_window->Swap();
  m_graphics->Update(time);
}

Graphics *Engine::getGraphics() { return this->m_graphics; }

static void cursorPositionCallBack(GLFWwindow *window, double xpos, double ypos)
{
  Engine *engine = (Engine *)(glfwGetWindowUserPointer(window));

  if (!engine)
    return;

  static float lastX = xpos;
  static float lastY = ypos;
  static bool firstMouse = true;

  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  Camera *camera = engine->getGraphics()->getCamera();
  if (!camera)
    return;

  camera->yaw += xoffset;
  camera->pitch += yoffset;

  if (camera->pitch > 89.0f)
    camera->pitch = 89.0f;
  if (camera->pitch < -89.0f)
    camera->pitch = -89.0f;
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  Engine *engine = (Engine *)(glfwGetWindowUserPointer(window));
  if (!engine)
    return;

  Camera *camera = engine->getGraphics()->getCamera();
  if (!camera)
    return;

  camera->fov -= (float)yoffset;
  if (camera->fov < 1.0f)
    camera->fov = 1.0f;
  if (camera->fov > 90.0f)
    camera->fov = 90.0f;
}
