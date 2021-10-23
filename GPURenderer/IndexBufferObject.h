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
			0.5f, 0.5f, 0.0f,   // 右上角
			0.5f, -0.5f, 0.0f,  // 右下角
			-0.5f, -0.5f, 0.0f, // 左下角
			-0.5f, 0.5f, 0.0f   // 左上角
		};

		unsigned int indices[] = { // 注意索引从0开始! 
			0, 1, 3, // 第一个三角形
			1, 2, 3  // 第二个三角形
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