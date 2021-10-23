#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SuikaGL
{
	class IndexBufferObject
	{
	public:
		IndexBufferObject();
		~IndexBufferObject();

		unsigned int EBO;
	private:

	};

	IndexBufferObject::IndexBufferObject()
	{
		float vertices[] = {
			0.5f, 0.5f, 0.0f,   // ���Ͻ�
			0.5f, -0.5f, 0.0f,  // ���½�
			-0.5f, -0.5f, 0.0f, // ���½�
			-0.5f, 0.5f, 0.0f   // ���Ͻ�
		};

		unsigned int indices[] = { // ע��������0��ʼ! 
			0, 1, 3, // ��һ��������
			1, 2, 3  // �ڶ���������
		};

		glGenBuffers(1, &EBO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	}

	IndexBufferObject::~IndexBufferObject()
	{
	}
}