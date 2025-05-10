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

void Object::Render(GLint posAttribLoc, GLint colAttribLoc,
										GLint tcAttribLoc, GLint hasTextureLoc,
										GLint nmAttribLoc, GLint hasNmap)
{
	glBindVertexArray(vao);
	// Enable vertex attibute arrays for each vertex attrib
	glEnableVertexAttribArray(posAttribLoc);
	glEnableVertexAttribArray(tcAttribLoc);
	glEnableVertexAttribArray(nmAttribLoc);

	// Bind your VBO
	glBindBuffer(GL_ARRAY_BUFFER, VB);

	// Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, vertex));
	glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoord));
	glVertexAttribPointer(nmAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

	if (m_texture != NULL)
		glUniform1i(hasTextureLoc, true);

	else
		glUniform1i(hasTextureLoc, false);

	if (m_normal_map != NULL)
		glUniform1i(hasNmap, true);

	else
		glUniform1i(hasNmap, false);

	// Bind your Element Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

	// Render
	if (instanced)
		glDrawElementsInstanced(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0, num_instances);
	else
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	// Disable vertex arrays
	glDisableVertexAttribArray(posAttribLoc);
	glDisableVertexAttribArray(tcAttribLoc);
	glDisableVertexAttribArray(nmAttribLoc);

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

void Object::Instance(std::vector<glm::mat4> &transforms)
{
	num_instances = transforms.size();
	instanced = true;

	glGenBuffers(1, &iVB);
	glBindBuffer(GL_ARRAY_BUFFER, iVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * num_instances, transforms.data(), GL_STATIC_DRAW);

	glBindVertexArray(this->vao);

	// looks weird but apparently need a pointer for each vector of the mat4
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(float) * i * 4));
		glVertexAttribDivisor(4 + i, 1);
	}

	glBindVertexArray(0);
}

void Object::UpdateInstanceBuffer(std::vector<glm::mat4> &transforms)
{
	glBindBuffer(GL_ARRAY_BUFFER, iVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * transforms.size(), transforms.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
