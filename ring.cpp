#include "ring.h"

Ring::Ring(int prec, const char *path)
{
  this->prec = prec;
  this->createVertices();
  this->InitBuffers();
  this->setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

  m_texture = new Texture(path);
  if (m_texture)
    hasTex = true;
  else
    hasTex = false;
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

void Ring::Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc)
{
  glBindVertexArray(vao);
  // Enable vertex attibute arrays for each vertex attrib
  glEnableVertexAttribArray(posAttribLoc);
  glEnableVertexAttribArray(colAttribLoc);
  glEnableVertexAttribArray(tcAttribLoc);

  // Bind your VBO
  glBindBuffer(GL_ARRAY_BUFFER, VB);

  // Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
  glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, vertex));
  glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoord));

  // If has texture, set up texture unit(s): update here for texture rendering
  if (m_texture != NULL)
  {
    glUniform1i(hasTextureLoc, true);
  }
  else
    glUniform1i(hasTextureLoc, false);

  // Bind your Element Array
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

  // Render
  glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

  // Disable vertex arrays
  glDisableVertexAttribArray(posAttribLoc);
  glDisableVertexAttribArray(colAttribLoc);
  glDisableVertexAttribArray(tcAttribLoc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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