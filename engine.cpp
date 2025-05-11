
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

  // main loop
  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
    double now = glfwGetTime();
    double dt = getDT();

    // starship speed
    float speed = 1.f;

    // get starship and camera for coordination
    Object *starship = m_graphics->getStarship();
    Camera *camera = m_graphics->getCamera();

    // check for input, if forward is pressed Move forward
    // since the starship has thrusters in the back dir, we only need forward
    // and mouse input
    ProcessInput();
    if (camera->forward)
      starship->Move(camera->GetFront(), speed, dt);

    // update camera
    m_graphics->getCamera()->Update(dt);

    // if first person is set, and theres a nearby obj, enter observation mode
    if (first_person && target)
    {
      // get target world pos (3rd col of model matrix)
      glm::vec3 planet_pos = target->GetModel()[3];
      // calculate an angle with time and scale to reduce orbital speed
      float angle = now * 0.2f;

      // if a target is set, scale orbital position to capture planet face.
      // else default first person, and orbit at an .25 y offset
      float scale = (target == nullptr) ? 4.5 : target->scale;
      glm::vec3 offset = glm::vec3(glm::cos(angle), .25f, glm::sin(angle)) * scale;

      // orbital position = position of the planet + to viewing offset
      glm::vec3 orbit_pos = planet_pos + offset;

      camera->position = orbit_pos;

      // set view to focus on the planet
      camera->SetView(glm::lookAt(orbit_pos, planet_pos, glm::vec3(0, 1, 0)));
    }
    else
    {
      // get camera position and front vector, position ship in front of cameras view (3rd person)
      glm::vec3 camera_pos = camera->GetPosition();
      glm::vec3 front = camera->GetFront();
      glm::vec3 ship_pos = camera_pos + front * 2.0f;

      // rotate ship tp face camera front
      glm::mat4 rotation = glm::lookAt(glm::vec3(0.0f), front, glm::vec3(0, 1, 0));
      // inevert view matrix to get model-space rotation to align cam
      rotation = glm::inverse(rotation);
      // rotate ship 180 to face forward
      rotation *= glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

      // translate, rotate scale
      glm::mat4 model = glm::translate(glm::mat4(1.0f), ship_pos) * rotation;
      model = glm::scale(model, glm::vec3(0.01f));

      // update starship model matrix
      starship->Update(model);
    }

    // display and poll
    Display(m_window->getWindow(), now);
    glfwPollEvents();
  }

  m_running = false;
}

void Engine::ProcessInput()
{
  // static bool to detect first person flag changes
  static bool space_pressed_last_frame = false;

  if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(m_window->getWindow(), true);

  // get camera, and if `w` is pressed, set forward flag with result
  Camera *camera = this->m_graphics->getCamera();
  camera->forward = glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS;

  // bool variable to make if look a little nicer
  bool space_pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS;

  // if space was pressed and wasnt pressed last frame
  if (space_pressed && !space_pressed_last_frame)
  {
    // toggle first person flag
    first_person = !first_person;
    // set camera first person flag
    camera->SetFirstPerson(first_person);

    // set orbiting flag in graphics (needed to stop rendering ship when orbiting)
    m_graphics->SetOrbit(first_person);

    // detect closest planet and assign target
    if (first_person)
      target = m_graphics->getClosestPlanet();
  }

  // update space_pressed
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

// cursor pos callback to look around
static void cursorPositionCallBack(GLFWwindow *window, double xpos, double ypos)
{
  // get engine ptr from window
  Engine *engine = (Engine *)(glfwGetWindowUserPointer(window));

  if (!engine)
    return;

  // arent staic variables cool?
  // track lastX and lastY, and check for first frame
  static float lastX = xpos;
  static float lastY = ypos;
  static bool firstMouse = true;

  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  // calculate offsets, for changes
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  // define sensitivity and scale
  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  // get camera
  Camera *camera = engine->getGraphics()->getCamera();
  if (!camera)
    return;

  // adjust pitch and yaw with offsets
  camera->yaw += xoffset;
  camera->pitch += yoffset;

  // limit pitch and yaw to prevent disorientation
  if (camera->pitch > 89.0f)
    camera->pitch = 89.0f;
  if (camera->pitch < -89.0f)
    camera->pitch = -89.0f;
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  // get Engine and Camera ptrs
  Engine *engine = (Engine *)(glfwGetWindowUserPointer(window));
  if (!engine)
    return;

  Camera *camera = engine->getGraphics()->getCamera();
  if (!camera)
    return;

  // increase and decrease fov with
  // not sure what xoffset is supposed to do but glfw gets mad if not there
  camera->fov -= (float)yoffset;
  if (camera->fov < 1.0f)
    camera->fov = 1.0f;
  if (camera->fov > 90.0f)
    camera->fov = 90.0f;
}
