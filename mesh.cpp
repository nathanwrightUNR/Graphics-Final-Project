#include "mesh.h"

Mesh::Mesh()
{
  this->Vertices.clear();
  this->Indices.clear();

  this->createVertices();
  this->InitBuffers();
  this->setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
}

Mesh::Mesh(glm::vec3 pivot, const char *fname)
{
  this->Vertices.clear();
  this->Indices.clear();

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
  hasNmap = false;
}

Mesh::Mesh(glm::vec3 pivot, const char *fname, const char *tname)
{
  this->Vertices.clear();
  this->Indices.clear();

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

  hasNmap = false;
}

bool Mesh::loadModelFromFile(const char *path)
{
  std::string str = path;
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

        glm::vec2 texcoord;

        if (mesh->HasTextureCoords(0))
        {
          tex = mesh->mTextureCoords[0][index];
          texcoord = glm::vec2(tex.x, tex.y);
        }
        else
        {
          texcoord = glm::vec2((pos.x + 1.0f) * 0.5f, (pos.z + 1.0f) * 0.5f);
        }

        glm::vec3 position(pos.x, pos.y, pos.z);
        glm::vec3 normal(norm.x, norm.y, norm.z);

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
