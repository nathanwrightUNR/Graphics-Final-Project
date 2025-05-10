#include "camera.h"

Camera::Camera() {}

Camera::~Camera() {}

bool Camera::Initialize(int w, int h)
{
  this->position = glm::vec3(0.0f, 9.0f, 55.0f);
  this->up = glm::vec3(0.0f, 1.0f, 0.0f);
  this->yaw = -90.0f;
  this->pitch = -15.0f;

  this->view = glm::lookAt(position, camera_front + position, up);

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

void Camera::Update(float dt)
{
  float speed = 10.0f;

  if (this->forward)
    this->position += speed * this->camera_front * dt;
  if (this->backward)
    this->position -= speed * this->camera_front * dt;
  if (this->left)
    this->position -= glm::normalize(glm::cross(this->camera_front, this->camera_up)) * speed * dt;
  if (this->right)
    this->position += glm::normalize(glm::cross(this->camera_front, this->camera_up)) * speed * dt;

  if (this->zoom_in)
    this->fov -= 20.0f * dt;
  if (this->zoom_out)
    this->fov += 20.0f * dt;

  if (this->fov < 1.0f)
    this->fov = 1.0f;
  if (this->fov > 90.0f)
    this->fov = 90.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  direction.y = sin(glm::radians(this->pitch));
  direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

  this->camera_front = glm::normalize(direction);
  this->camera_right = glm::normalize(glm::cross(this->camera_front, this->up));
  this->camera_up = glm::normalize(glm::cross(this->camera_right, this->camera_front));

  view = glm::lookAt(this->position, this->position + this->camera_front, this->camera_up);

  projection = glm::perspective(glm::radians(this->fov), 800.0f / 600.0f, 0.01f, 100.0f);
}

glm::vec3 Camera::GetPosition()
{
  return this->position;
}

glm::vec3 Camera::GetFront()
{
  return this->camera_front;
}