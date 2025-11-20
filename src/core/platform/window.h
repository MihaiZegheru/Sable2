#ifndef SRC_WINDOW_WINDOW_H__
#define SRC_WINDOW_WINDOW_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Window
{
public:
	Window(int windowWidth, int windowHeight, const char* windowName, bool fullscreen = false);
	~Window();

	inline GLFWwindow* GetInstance() const { return m_GlfwWindow; }

	int Destroy();

private:
	GLFWwindow* m_GlfwWindow;

	int m_windowWidth;
	int m_windowHeight;
	const char* m_windowName;
	bool m_fullscreen;

	int InitGlfw();
	int InitGlad();
	int CreateWindow();
};


#endif  // SRC_WINDOW_WINDOW_H__
