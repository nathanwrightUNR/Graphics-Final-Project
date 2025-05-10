#include "sphere.h"

Sphere::Sphere(int prec, float angle, float scale, const char *tf)
{
  this->prec = prec;
  this->angle = angle;
  this->scale = scale;
  createVertices();
  InitBuffers();
  setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

  // load texture from file
  m_texture = new Texture(tf);
  if (m_texture)
    hasTex = true;
  else
    hasTex = false;

  hasNmap = false;
}

Sphere::Sphere(int prec, float angle, float scale, const char *tf, const char *nf)
{
  this->prec = prec;
  this->angle = angle;
  this->scale = scale;
  createVertices();
  InitBuffers();
  setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

  // load texture from file
  m_texture = new Texture(tf);
  if (m_texture)
    hasTex = true;
  else
    hasTex = false;

  m_normal_map = new Texture(nf);
  if (m_normal_map)
    hasNmap = true;
  else
    hasNmap = false;
}

void Sphere::createVertices()
{
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;

  for (int i = 0; i <= prec; i++)
  {
    for (int j = 0; j <= prec; j++)
    {
      float y = glm::cos(glm::radians(180.f - i * 180.f / prec));
      float x = -glm::cos(glm::radians(j * 360.f / prec)) * glm::abs(glm::cos(glm::asin(y)));
      float z = glm::sin(glm::radians(j * 360.f / prec)) * glm::abs(glm::cos(glm::asin(y)));

      vertices.push_back(glm::vec3(x, y, z));
      texCoords.push_back(glm::vec2((float)j / prec, (float)i / prec));
      normals.push_back(glm::vec3(x, y, z));
    }
  }

  for (int i = 0; i < prec; i++)
  {
    for (int j = 0; j < prec; j++)
    {
      this->Indices.push_back(i * (prec + 1) + j);
      this->Indices.push_back(i * (prec + 1) + j + 1);
      this->Indices.push_back((i + 1) * (prec + 1) + j);
      this->Indices.push_back(i * (prec + 1) + j + 1);
      this->Indices.push_back((i + 1) * (prec + 1) + j + 1);
      this->Indices.push_back((i + 1) * (prec + 1) + j);
    }
  }

  for (int i = 0; i < vertices.size(); i++)
  {
    this->Vertices.push_back(Vertex(vertices[i], normals[i], texCoords[i]));
  }
}
