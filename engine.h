#ifndef ENGINE_H
#define ENGINE_H

#include <assert.h>
#include "window.h"
#include "graphics.h"

static void cursorPositionCallBack(GLFWwindow *, double xpos, double ypos);

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

/*
 * Enigne class, instantiatates core
 * components to the OpenGL workflow
 */

class Engine
{
public:
  /**
   * @brief construct a new Engine object
   * Enigine constuctor, with window params.
   *
   * @param name
   * @param width
   * @param height
   */
  Engine(const char *name, int width, int height);

  // engine desturctor.
  ~Engine();

  /**
   * @brief
   * initialize engine by
   * instantiating Window and Graphics
   * objects.
   *
   * @return bool
   */
  bool Initialize();

  /**
   * @brief
   * main render loop, processes user
   * input, calls Display(GLFWwindow*, double),
   * and polls for events for the next iteration.
   */
  void Run();

  /**
   * @brief
   * takes a user input assigned to a given key.
   * escape closes the window, while UDLR adjusts
   * the speed in a given direction, or angle of
   * rotation.
   */
  void ProcessInput();

  /**
   * @brief
   * returns glfwGetTime(), representing delta
   * time.
   *
   * @return unsigned int
   */
  float getDT();

  // return m_graphics for cursor callback
  Graphics *getGraphics();

  /**
   * @brief get the current Time Millis object.
   *
   * @return long long
   */
  long long GetCurrentTimeMillis();

  /**
   * @brief
   * Render graphics object, swap windows to avoid
   * flickering, and update with dt.
   */
  void Display(GLFWwindow *, double);

private:
  double last_time = 0.0;
  // window variables.
  Window *m_window;
  const char *m_WINDOW_NAME;
  int m_WINDOW_WIDTH;
  int m_WINDOW_HEIGHT;
  bool m_FULLSCREEN;

  // pointer to Graphics object.
  Graphics *m_graphics;

  // running flag.
  bool m_running;

  // new flag for first person
  bool first_person = false;

  // pointer to closes celestial obj
  // if not within certain min distance its nullptr
  Object *target;
};

#endif // ENGINE_H
