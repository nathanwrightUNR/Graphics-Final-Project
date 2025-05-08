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

void Object::Render(GLint posAttribLoc, GLint colAttribLoc)
{
	glBindVertexArray(vao);
	// Enable Vertext Attributes
	glEnableVertexAttribArray(posAttribLoc);
	glEnableVertexAttribArray(colAttribLoc);

	// Bind your VBO buffer(s) and then setup vertex attribute pointers
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, vertex));
	glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

	// Bind your index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

	// Render
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	// Disable Vertex Attribuates
	glDisableVertexAttribArray(posAttribLoc);
	glDisableVertexAttribArray(colAttribLoc);
}

void Object::Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTex)
{
	Render(posAttribLoc, colAttribLoc);
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
