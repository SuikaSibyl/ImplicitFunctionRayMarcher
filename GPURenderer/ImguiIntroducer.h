#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace SuikaGL
{
	const char* glsl_version = "#version 130";
     
	class ImguiIntroducer
	{
	public:
		ImguiIntroducer(GLFWwindow* window);
		~ImguiIntroducer();
        
        bool doRotate = false;
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 lightColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        int subnum = 0;
        int demoID = 0;

        int maxShaderId = 1;
        int shaderId = 0;
		void Draw()
		{

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            //if (show_demo_window)
            //    ImGui::ShowDemoWindow(&show_demo_window);

            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Para panel");                          // Create a window called "Hello, world!" and append into it.
                
                ImGui::Text("Welcom to implicit Surface Demo by Haolin Lu.");               // Display some text (you can use a format strings too)

                if (ImGui::Button("Change Shader"))
                {
                    shaderId++;
                    if (shaderId > maxShaderId)
                        shaderId = 0;
                }
                ImGui::SameLine();
                if (shaderId == 0)
                {
                    ImGui::Text("Surface Shader");
                }
                else if (shaderId == 1)
                {
                    ImGui::Text("Glass Shader");
                }

                if (ImGui::Button("Change Demo"))
                {
                    demoID++;
                    if (demoID > 3)
                        demoID = 0;
                }

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	private:
        GLFWwindow* window;
	};

	ImguiIntroducer::ImguiIntroducer(GLFWwindow* window)
	{    
        this->window = window;
        // imgui: setup dear ImGui context
	    // ---------------------------------------
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	ImguiIntroducer::~ImguiIntroducer()
	{
	}
}