#include "ImguiIntroducer.h"
#include "GlfwIntroducer.h"
#include "VertexArrayObject.h"

#include <stdio.h>
#include <iostream>
#include "Shader.h"
#include "CounterModel.h"
#include "Camera.h"
#include "ScreenQuad.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Metaball.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int metaball_id = 0;

SuikaGL::Metaball metaball_rabbit("./Resources/datas/data_rabbit.txt");
SuikaGL::Metaball metaball_dragon("./Resources/datas/data_dragon.txt");
SuikaGL::Metaball metaball_cow("./Resources/datas/data_cow.txt");
SuikaGL::Metaball metaball_fit("./Resources/datas/data_backup.txt");

SuikaGL::Metaball* metaball_now = &metaball_rabbit;
SuikaGL::Metaball* metaball_next = &metaball_dragon;

void ChangeMetaball()
{
    if (metaball_id == 0)
    {
        metaball_id = 1;
        metaball_now = &metaball_dragon;
        metaball_next = &metaball_cow;
    }
    else if (metaball_id == 1)
    {
        metaball_id = 2;
        metaball_now = &metaball_cow;
        metaball_next = &metaball_rabbit;
    }
    else 
    {
        metaball_id = 0;
        metaball_now = &metaball_rabbit;
        metaball_next = &metaball_dragon;
    }
}
int main()
{
    GLFWwindow* window = NULL;

    SuikaGL::GlfwIntroducer glfwIntroducer(&window, &SCR_WIDTH, &SCR_HEIGHT);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    SuikaGL::ImguiIntroducer imguiIntroducer(window);

    SuikaGL::Shader subdivisionShader("lit.vert", "lit.frag");
    SuikaGL::Shader marchingfitShader("marching.vert", "marching.frag");
    SuikaGL::Shader marchingShader("marching.vert", "ImplicitSurface.frag");
    SuikaGL::Shader marchingShader2("marching.vert", "ImplicitSurface2.frag");
    SuikaGL::Shader marchingShader3("marching.vert", "ImplicitSurface3.frag");
    SuikaGL::Shader marchingShader4("marching.vert", "ImplicitSurface4.frag");
    SuikaGL::Shader screenShader("screendrawer.vert", "screendrawer.frag");  
    SuikaGL::Shader skyboxShader("skybox.vert", "skybox.frag");

    SuikaGL::Cubemap skybox("skybox");

    SuikaGL::Metaball metaball = metaball_now->blend(*metaball_next,0);
    //metaball = metaball_rabbit;
    //SuikaGL::VertexArrayObject triangle;

    Model cube(("./Resources/models/cube.obj"));
    Model test(("./Resources/models/venusv.obj"));
    Model venus(("./Resources/models/venusv.obj"));
    SuikaGL::ScreenQuad screenQuad;

    //metaball = metaball_rabbit.blend(metaball_dragon, 0.5);

    glEnable(GL_DEPTH_TEST);
    float angle = 0;
    float accumulate_time = 0;
    float alpha = 0;
    bool direction = true;
    SuikaGL::Framebuffer framebuffer(&SCR_WIDTH, &SCR_HEIGHT);

    // Post Processing Shaders
    SuikaGL::Shader pp_vigneteShader("postprocess.vert", "postprocess_vignete.frag");
    pp_vigneteShader.use();
    pp_vigneteShader.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
    framebuffer.PushPostProcess(pp_vigneteShader);

    SuikaGL::Texture texture("test.jpg");
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        glm::vec3 lightPos(1.2f, 1.0f, -1.0f);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SuikaGL::Shader* mainShader = NULL;
        mainShader = &subdivisionShader;


        framebuffer.UseFrameBuffer();

        // don't forget to enable shader before setting uniforms
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Draw Sky Box
        skyboxShader.use();
        glm::mat4 sky_view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", sky_view);
        skyboxShader.setMat4("projection", projection);
        skybox.DrawSkybox(skyboxShader);

        if (imguiIntroducer.demoID == 0)
        {
            glEnable(GL_DEPTH_TEST);
            // 3D Morphing Demo
            accumulate_time += deltaTime;
            if (accumulate_time > 0.05f)
            {
                metaball = metaball_now->blend(*metaball_next, alpha);
                accumulate_time -= 0.05f;

                alpha += 0.01f;

                if (alpha >= 1.0f)
                {
                    accumulate_time -= 1.0f;
                    ChangeMetaball();
                    alpha = 0;
                }
            }
            // Draw 3D Morphing
            marchingShader.use();
            skybox.BindTexture(marchingShader);
            marchingShader.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            marchingShader.setVec3("camPos", camera.Position);
            marchingShader.setMat4("invView", glm::inverse(view));
            marchingShader.setMat4("invProjection", glm::inverse(projection));
            marchingShader.setMat4("ModelViewProjection", projection * view);
            marchingShader.setFloat("currentFrame", currentFrame);
            marchingShader.setVec4Arr("blobs", *(metaball.pointer), metaball.num);
            if (imguiIntroducer.shaderId == 0)
            {
                marchingShader.setBool("useGlass", false);
            }
            else
            {
                marchingShader.setBool("useGlass", true);
            }
            screenQuad.Draw(marchingShader);
        }
        else if (imguiIntroducer.demoID == 1)
        {
            marchingShader2.use();
            skybox.BindTexture(marchingShader2);
            marchingShader2.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            marchingShader2.setVec3("camPos", camera.Position);
            marchingShader2.setMat4("invView", glm::inverse(view));
            marchingShader2.setMat4("invProjection", glm::inverse(projection));
            marchingShader2.setMat4("ModelViewProjection", projection * view);
            marchingShader2.setFloat("currentFrame", currentFrame);
            marchingShader2.setVec4Arr("blobs", *(metaball.pointer), metaball.num);
            if (imguiIntroducer.shaderId == 0)
            {
                marchingShader2.setBool("useGlass", false);
            }
            else
            {
                marchingShader2.setBool("useGlass", true);
            }
            screenQuad.Draw(marchingShader2);
        }
        else if (imguiIntroducer.demoID == 2)
        {
            marchingShader3.use();
            skybox.BindTexture(marchingShader3);
            marchingShader3.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            marchingShader3.setVec3("camPos", camera.Position);
            marchingShader3.setMat4("invView", glm::inverse(view));
            marchingShader3.setMat4("invProjection", glm::inverse(projection));
            marchingShader3.setMat4("ModelViewProjection", projection * view);
            marchingShader3.setFloat("currentFrame", currentFrame);
            marchingShader3.setVec4Arr("blobs", *(metaball.pointer), metaball.num);
            if (imguiIntroducer.shaderId == 0)
            {
                marchingShader3.setBool("useGlass", false);
            }
            else
            {
                marchingShader3.setBool("useGlass", true);
            }
            screenQuad.Draw(marchingShader3);
        }
        else if (imguiIntroducer.demoID == 3)
        {
            marchingShader4.use();
            skybox.BindTexture(marchingShader4);
            marchingShader4.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            marchingShader4.setVec3("camPos", camera.Position);
            marchingShader4.setMat4("invView", glm::inverse(view));
            marchingShader4.setMat4("invProjection", glm::inverse(projection));
            marchingShader4.setMat4("ModelViewProjection", projection * view);
            marchingShader4.setFloat("currentFrame", currentFrame);
            marchingShader4.setVec4Arr("blobs", *(metaball.pointer), metaball.num);
            if (imguiIntroducer.shaderId == 0)
            {
                marchingShader4.setBool("useGlass", false);
            }
            else
            {
                marchingShader4.setBool("useGlass", true);
            }
            screenQuad.Draw(marchingShader4);
        }
        else if (imguiIntroducer.demoID == 4)
        {

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(0.0f, -1.0f, -4.0f)); // translate it down so it's at the center of the scene
            model = glm::scale(model, glm::vec3(1.0f / 4000, 1.0f / 4000, 1.0f / 4000));	// it's a bit too big for our scene, so scale it down
            //model = glm::rotate(model, angle, glm::vec3(0, 1, 1));
            mainShader->use();
            glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(view * model)));
            mainShader->setMat4("projection", projection);
            mainShader->setMat4("view", view);
            mainShader->setMat3("normal", normal);
            mainShader->setMat4("model", model);
            mainShader->setInt("subnum", imguiIntroducer.subnum);

            test.Draw(*mainShader);

            glDisable(GL_DEPTH_TEST);
            // Draw 3D Morphing
            marchingShader.use();
            skybox.BindTexture(marchingShader);
            marchingShader.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            marchingShader.setVec3("camPos", camera.Position);
            marchingShader.setMat4("invView", glm::inverse(view));
            marchingShader.setMat4("invProjection", glm::inverse(projection));
            marchingShader.setMat4("ModelViewProjection", projection* view);
            marchingShader.setFloat("currentFrame", currentFrame);
            marchingShader.setVec4Arr("blobs", *(metaball_fit.pointer), metaball_fit.num);
            if (imguiIntroducer.shaderId == 0)
            {
                marchingShader.setBool("useGlass", false);
            }
            else
            {
                marchingShader.setBool("useGlass", true);
            }
            screenQuad.Draw(marchingShader);
        }

        // Do Post Proccessing
        framebuffer.PostProcess();

        framebuffer.UseDefaultBuffer(screenShader);
        screenShader.use();
        screenQuad.Draw(screenShader);

        // draw ui
        // -----------
        imguiIntroducer.Draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ------------------------------------------------------------------
    // Clean up
    // ------------------------------------------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    // imgui: terminate, clean up
    // --------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        if (camera.inControl == true)
        {
            camera.inControl = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            camera.inControl = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}