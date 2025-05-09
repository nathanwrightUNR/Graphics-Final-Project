#include "mesh.h"

Mesh::Mesh()
{
  this->createVertices();
  this->InitBuffers();
  this->setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
}

Mesh::Mesh(glm::vec3 pivot, const char *fname)
{
  // Vertex Set Up
  this->loadModelFromFile(fname);

  // Model Set Up
  angle = 0.0f;
  pivotLocation = pivot;
  model = glm::translate(glm::mat4(1.0f), pivotLocation);

  // Buffer Set Up
  if (!InitBuffers())
  {
    printf("some buffers not initialized.\n");
  }

  hasTex = false;
}

Mesh::Mesh(glm::vec3 pivot, const char *fname, const char *tname)
{
  // Vertex Set Up
  this->loadModelFromFile(fname);

  // Model Set Up
  angle = 0.0f;
  pivotLocation = pivot;
  model = glm::translate(glm::mat4(1.0f), pivotLocation);

  // Buffer Set Up
  if (!InitBuffers())
  {
    printf("some buffers not initialized.\n");
  }

  // load texture from file
  m_texture = new Texture(tname);
  if (m_texture)
    hasTex = true;
  else
    hasTex = false;
}

bool Mesh::loadModelFromFile(const char *path)
{
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

  if (!scene)
  {
    printf("couldn't open the .obj file. \n");
    return false;
  }

  for (int i = 0; i < scene->mNumMeshes; i++)
  {
    aiMesh *mesh = scene->mMeshes[i];
    int iMeshFaces = mesh->mNumFaces;
    for (int j = 0; j < iMeshFaces; j++)
    {
      const aiFace &face = mesh->mFaces[j];
      for (int k = 0; k < 3; k++)
      {
        unsigned int index = face.mIndices[k];
        aiVector3D pos = mesh->mVertices[index];
        aiVector3D norm = mesh->HasNormals() ? mesh->mNormals[index] : aiVector3D(0.0f, 0.0f, 0.0f);
        aiVector3D tex(0.0f, 0.0f, 0.0f);
        if (mesh->HasTextureCoords(0))
        {
          tex = mesh->mTextureCoords[0][index];
        }

        glm::vec3 position(pos.x, pos.y, pos.z);
        glm::vec3 normal(norm.x, norm.y, norm.z);
        glm::vec2 texcoord(tex.x, tex.y);

        this->Vertices.push_back(Vertex(position, normal, texcoord));
      }
    }
  }

  for (unsigned int i = 0; i < Vertices.size(); ++i)
  {
    Indices.push_back(i);
  }

  return true;
}

void Mesh::createVertices()
{
  this->loadModelFromFile("../assets/SpaceShip-1/SpaceShip-1.obj");
}
