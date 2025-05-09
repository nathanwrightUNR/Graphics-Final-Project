#include "ring.h"

Ring::Ring(int prec, float inner_r, float outer_r)
{
  this->prec = prec;
  this->inner_r = inner_r;
  this->outer_r = outer_r;
  this->createVertices();
  this->InitBuffers();
  this->setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

  hasTex = false;
  hasNmap = false;
}

Ring::Ring(int prec, float inner_r, float outer_r, const char *tf, const char *nf)
{
  this->prec = prec;
  this->inner_r = inner_r;
  this->outer_r = outer_r;
  this->createVertices();
  this->InitBuffers();
  this->setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

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

Ring::Ring(int prec, float inner_r, float outer_r, const char *path)
{
  this->prec = prec;
  this->inner_r = inner_r;
  this->outer_r = outer_r;
  this->createVertices();
  this->InitBuffers();
  this->setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

  m_texture = new Texture(path);
  if (m_texture)
    hasTex = true;
  else
    hasTex = false;
}

void Ring::createVertices()
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;

  for (int i = 0; i <= prec; i++)
  {
    float angle = glm::radians(i * 360.0f / prec);
    float _cos = glm::cos(angle);
    float _sin = glm::sin(angle);

    vertices.push_back(glm::vec3(_cos * outer_r, _sin * outer_r, 0.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    texCoords.push_back(glm::vec2(1.0f, (float)i / prec));

    vertices.push_back(glm::vec3(_cos * inner_r, _sin * inner_r, 0.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    texCoords.push_back(glm::vec2(0.0f, (float)i / prec));
  }

  for (int i = 0; i < prec; i++)
  {
    int start = i * 2;
    this->Indices.push_back(start);
    this->Indices.push_back(start + 1);
    this->Indices.push_back(start + 2);

    this->Indices.push_back(start + 1);
    this->Indices.push_back(start + 3);
    this->Indices.push_back(start + 2);
  }

  for (int i = 0; i < vertices.size(); ++i)
  {
    this->Vertices.push_back(Vertex(vertices[i], normals[i], texCoords[i]));
  }
}