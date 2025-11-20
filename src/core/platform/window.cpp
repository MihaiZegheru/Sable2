#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


Window::Window(int windowWidth, int windowHeight, const char* windowName, bool fullscreen)
{
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
    m_windowName = windowName;
    m_fullscreen = fullscreen;
    InitGlfw();
    CreateWindow();
    InitGlad();
}

Window::~Window()
{
    Destroy();
}

int Window::CreateWindow()
{
    // Always create a windowed mode window (monitor = nullptr)
    m_GlfwWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowName, nullptr, nullptr);

    if (m_GlfwWindow == nullptr) {
        glfwTerminate();
        return -1;
    }

    // If fullscreen flag is set, maximize the window to cover the screen
    if (m_fullscreen)
    {
        glfwMaximizeWindow(m_GlfwWindow);
    }

    glfwMakeContextCurrent(m_GlfwWindow);

    return 0;
}

int Window::InitGlfw()
{
    int err = glfwInit();
    if (!err)
    {
        glfwTerminate();
        return err;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return 0;
}

int Window::InitGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        //glfwTerminate();
        return -1;
    }
    if (!GLAD_GL_ARB_direct_state_access) {
        glfwTerminate();
        return -1;
    }

    return 0;
}

int Window::Destroy()
{
    glfwDestroyWindow(m_GlfwWindow);
    glfwTerminate();

    return 0;
}
