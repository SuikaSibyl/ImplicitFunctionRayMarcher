#pragma once

#include <iostream>
#include <glad/glad.h> 
#include <vector>
#include "Shader.h"
#include "ScreenQuad.h"

namespace SuikaGL
{
	class Framebuffer
	{
	public:
		Framebuffer(unsigned int* SCR_WIDTH, unsigned int* SCR_HEIGHT)
		{
			glGenFramebuffers(1, &framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			// Create Texture
			glGenTextures(1, &texColorBuffer);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *SCR_WIDTH, *SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Create Texture
			glGenTextures(1, &texColorBuffer_back);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer_back);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *SCR_WIDTH, *SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Bind the texture to Frame Buffer
			ChangeTextureBind();

			// Add Depth & Stencil Buffer
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *SCR_WIDTH, *SCR_HEIGHT);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void UpdateBufferSize(unsigned int* SCR_WIDTH, unsigned int* SCR_HEIGHT)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			// Create Texture
			glGenTextures(1, &texColorBuffer);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *SCR_WIDTH, *SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Bind the texture to Frame Buffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

			// Add Depth & Stencil Buffer
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *SCR_WIDTH, *SCR_HEIGHT);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void UseFrameBuffer()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
		}

		void UseDefaultBuffer(Shader& shader)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			shader.use();
			glBindTexture(GL_TEXTURE_2D, bindBuffer);
		}

		void PushPostProcess(Shader& shader)
		{
			postProcessQueue.push_back(shader);
		}

		void PostProcess()
		{
			glDisable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			for (auto iter = postProcessQueue.begin(); iter != postProcessQueue.end(); iter++)
			{
				ChangeTextureBind();
				iter->use();
				glBindTexture(GL_TEXTURE_2D, idleBuffer);
				screenQuad.Draw(*iter);
			}
		}
	private:
		bool usebuffer = false;

		unsigned int framebuffer;
		unsigned int texColorBuffer;
		unsigned int texColorBuffer_back;

		unsigned int bindBuffer;
		unsigned int idleBuffer;

		vector<Shader> postProcessQueue;
		SuikaGL::ScreenQuad screenQuad;

		void ChangeTextureBind()
		{
			usebuffer = !usebuffer;
			if (usebuffer)
			{
				// Bind the texture to Frame Buffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
				bindBuffer = texColorBuffer;
				idleBuffer = texColorBuffer_back;
			}
			else
			{
				// Bind the texture to Frame Buffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer_back, 0);
				bindBuffer = texColorBuffer_back;
				idleBuffer = texColorBuffer;
			}
		}
	};
}