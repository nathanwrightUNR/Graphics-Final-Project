#include "camera.h"

Camera::Camera() {}

Camera::~Camera() {}

bool Camera::Initialize(int w, int h)
{
  // init at the edge of the system
  this->position = glm::vec3(0.0f, 9.0f, 60.0f);
  this->up = glm::vec3(0.0f, 1.0f, 0.0f);

  // init view
  this->view = glm::lookAt(position, camera_front + position, up);

  // init perspective
  this->projection = glm::perspective(glm::radians(this->fov),
                                      float(w) / float(h),
                                      0.01f,
                                      100.0f);

  return true;
}

glm::mat4 Camera::GetProjection()
{
  return this->projection;
}

glm::mat4 Camera::GetView()
{
  return this->view;
}

// update camera pos and orientation
void Camera::Update(float dt)
{
  float speed = 10.0f;

  // move camera forward along front if user presses 'w'
  if (this->forward)
    this->position += speed * this->camera_front * dt;

  // if zoom in callback is triggered reduce or increase fov
  if (this->zoom_in)
    this->fov -= 20.0f * dt;
  if (this->zoom_out)
    this->fov += 20.0f * dt;

  // limits fov
  if (this->fov < 1.0f)
    this->fov = 1.0f;
  if (this->fov > 90.0f)
    this->fov = 90.0f;

  // calculate new direction from yaw and pitch
  glm::vec3 direction;
  direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  direction.y = sin(glm::radians(this->pitch));
  direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

  // update orientation vectors
  this->camera_front = glm::normalize(direction);
  this->camera_right = glm::normalize(glm::cross(this->camera_front, this->up));
  this->camera_up = glm::normalize(glm::cross(this->camera_right, this->camera_front));

  // update view matrix
  view = glm::lookAt(this->position, this->position + this->camera_front, this->camera_up);

  // update projection matrix
  projection = glm::perspective(glm::radians(this->fov), 800.0f / 600.0f, 0.01f, 100.0f);
}

// return cam pos
glm::vec3 Camera::GetPosition()
{
  return this->position;
}

// get cam front vec
glm::vec3 Camera::GetFront()
{
  return this->camera_front;
}

// if first person is enabled reduce fov (zoom)
void Camera::SetFirstPerson(bool enable)
{
  fov = enable ? base_fov * 0.5f : base_fov;
}

// update view called from engine to init observation mode
void Camera::SetView(glm::mat4 v)
{
  this->view = v;
}