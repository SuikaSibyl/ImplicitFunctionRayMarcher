#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexBufferObject.h"
#include "IndexBufferObject.h"

namespace SuikaGL
{
	class VertexArrayObject
	{
	public:
		VertexArrayObject(float* vertices, int size);
		~VertexArrayObject();

		void Draw();
		unsigned int VAO;

	private:
		int vertexNum;
	};

	VertexArrayObject::VertexArrayObject(float* vertices, int size)
	{
		vertexNum = size / 3;
		glGenVertexArrays(1, &VAO);

		// 1. ��VAO
		glBindVertexArray(VAO);
		// 2. �Ѷ������鸴�Ƶ������й�OpenGLʹ��
		VertexBufferObject vertexBufferObject(vertices, size);
		//IndexBufferObject indexBufferObject;
		// 3. ���ö�������ָ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	VertexArrayObject::~VertexArrayObject()
	{
	}

	void VertexArrayObject::Draw()
	{
		// Draw VAO object
		// ---------------
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertexNum);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}