#include "ring.h"

// consturctor for basic ring
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

// constructor for ring w texture and normal map
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

// constructor for ring with only texture
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

// procedurally generate ring
void Ring::createVertices()
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;

  // basically you do triangles opositely orented and twist it into a circle
  for (int i = 0; i <= prec; i++)
  {
    float angle = glm::radians(i * 360.0f / prec);

    // outer vertices at r = outer_r
    vertices.push_back(glm::vec3(glm::cos(angle) * outer_r, glm::sin(angle) * outer_r, 0.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    texCoords.push_back(glm::vec2(1.0f, (float)i / prec));

    // inner vertices at r = inner_r
    vertices.push_back(glm::vec3(glm::cos(angle) * inner_r, glm::sin(angle) * inner_r, 0.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    texCoords.push_back(glm::vec2(0.0f, (float)i / prec));
  }

  // calcualte indices by connecting triangle like ths:
  /*
   *  ______
   *  \    //\
   *   \  //  \
   *    \//____\
   */
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

  // store in Vertex instances and pushback
  for (int i = 0; i < vertices.size(); ++i)
  {
    this->Vertices.push_back(Vertex(vertices[i], normals[i], texCoords[i]));
  }
}