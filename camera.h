#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
public:
  Camera();
  ~Camera();
  bool Initialize(int w, int h);
  glm::mat4 GetProjection();
  glm::vec3 GetPosition();
  glm::mat4 GetView();
  void Update(float dt);

  // get cameras front vector for ship cam
  glm::vec3 GetFront();

  // boolean flags for callbacks and movement
  bool forward = false;
  bool zoom_in = false;
  bool zoom_out = false;

  // fov, pitch and yaw
  float fov = 50.0f;
  float base_fov = 50.0f;

  float pitch = -15.0f;
  float yaw = -90.0f;

  // camera poisition and vectors
  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 camera_right;
  glm::vec3 camera_front;
  glm::vec3 camera_up;

  // function to set first person, reduces fov to zoom
  void SetFirstPerson(bool enable);

  // setter for the view matrix, needed for observation mode
  void SetView(glm::mat4);

private:
  glm::mat4 projection;
  glm::mat4 view;
};

#endif /* CAMERA_H */
