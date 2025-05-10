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
  glm::vec3 GetFront();

  bool forward = false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool zoom_in = false;
  bool zoom_out = false;

  float fov = 40.0f;
  float pitch = 0.0f;
  float yaw = -90.0f;

  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 camera_right;
  glm::vec3 camera_front;
  glm::vec3 camera_up;

  float base_fov = 40.0f;

  void SetFirstPerson(bool enable);

private:
  glm::mat4 projection;
  glm::mat4 view;
};

#endif /* CAMERA_H */
