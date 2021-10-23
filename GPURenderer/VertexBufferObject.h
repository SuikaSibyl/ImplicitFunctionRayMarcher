#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SuikaGL
{
	class VertexBufferObject
	{
	public:
		VertexBufferObject(float* vertices, int size);
		~VertexBufferObject();

	private:
		unsigned int VBO;
	};

	VertexBufferObject::VertexBufferObject(float* vertices, int size)
	{
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);
	}

	VertexBufferObject::~VertexBufferObject()
	{
	}
}