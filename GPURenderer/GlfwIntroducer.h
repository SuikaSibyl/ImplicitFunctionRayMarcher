#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace SuikaGL
{
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    // settings
    unsigned int* SCR_WIDTH;
    unsigned int* SCR_HEIGHT;

	class GlfwIntroducer
	{
	public:
		GlfwIntroducer(GLFWwindow** window, unsigned int* Width, unsigned int* Height);
		~GlfwIntroducer();

	private:

	};

	GlfwIntroducer::GlfwIntroducer(GLFWwindow** window, unsigned int* Width, unsigned int* Height)
	{
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw: window creation
        // --------------------
        SCR_WIDTH = Width;
        SCR_HEIGHT = Height;
        *window = glfwCreateWindow(*SCR_WIDTH, *SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (*window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(*window);

        glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
	}

	GlfwIntroducer::~GlfwIntroducer()
	{
	}

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        *SCR_WIDTH = width;
        *SCR_HEIGHT = height;
        glViewport(0, 0, width, height);
    }

}