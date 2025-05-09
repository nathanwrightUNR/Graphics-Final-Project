#include "object.h"

Object::~Object()
{
	// clear Vertices and Indices vectors
	Vertices.clear();
	Indices.clear();
}

// update model matrix
void Object::Update(glm::mat4 model_matrix)
{
	this->model = model_matrix;
}

glm::mat4 Object::GetModel()
{
	// return model matrix
	return model;
}

void Object::Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc)
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

bool Object::InitBuffers()
{
	// For OpenGL 3
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return true;
}

void Object::setupModelMatrix(glm::vec3 pivot, float angle, float scale)
{
	pivotLocation = pivot;
	model = glm::translate(glm::mat4(1.0f), pivotLocation);
	model *= glm::rotate(glm::mat4(1.f), angle, glm::vec3(0., 1., 0));
	model *= glm::scale(glm::vec3(scale, scale, scale));
}
