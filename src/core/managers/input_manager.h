#ifndef SRC_INPUTMANAGER_H__
#define SRC_INPUTMANAGER_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace core::managers {

class InputManager
{
public:
	static InputManager& GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	void Init(GLFWwindow* window)
	{
		// Set up mouse button callback
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		// Set up scroll callback
		glfwSetScrollCallback(window, ScrollCallback);
		// Store window pointer for callbacks
		glfwSetWindowUserPointer(window, this);
	}

	void Listen(GLFWwindow* window)
	{
        // Vertical Axis
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_vertical = 1;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
        {
            if (m_vertical > 0)
            {
                m_vertical = 0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_vertical = -1;
        }
        else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
        {
            if (m_vertical < 0)
            {
                m_vertical = 0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_vertical = 0;
        }

        // Horizontal Axis
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_horizontal = 1;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
        {
            if (m_horizontal > 0)
            {
                m_horizontal = 0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_horizontal = -1;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
        {
            if (m_horizontal < 0)
            {
                m_horizontal = 0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_horizontal = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            m_jump = true;
        }
        else
        {
            m_jump = false;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            m_leftshift = true;
        }
        else
        {
            m_leftshift = false;
        }

		m_pressedD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !m_holdD;
		m_pressedA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !m_holdA;
		m_holdD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		m_holdA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        m_mousePosition.x = mouseX;
        m_mousePosition.y = mouseY;

		// Update mouse button states
		UpdateMouseButton(window, GLFW_MOUSE_BUTTON_LEFT, m_leftMouseButton, m_leftMouseButtonPressed, m_leftMouseButtonReleased);
		UpdateMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT, m_rightMouseButton, m_rightMouseButtonPressed, m_rightMouseButtonReleased);
		UpdateMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE, m_middleMouseButton, m_middleMouseButtonPressed, m_middleMouseButtonReleased);

		// Reset scroll delta at the end of the frame
		m_scrollDelta = glm::vec2(0.0f, 0.0f);
	}

    float GetAxis(std::string axis)
    {
        if (axis == "horizontal")
        {
            return m_horizontal;
        }
        if (axis == "vertical")
        {
            return m_vertical;
        }
    }

    glm::vec2 GetMousePosition()
    {
        return m_mousePosition;
    }

	// Mouse button state (held down)
	bool GetMouseButton(int button)
	{
		switch (button)
		{
		case 0: return m_leftMouseButton;
		case 1: return m_rightMouseButton;
		case 2: return m_middleMouseButton;
		default: return false;
		}
	}

	// Mouse button pressed this frame
	bool GetMouseButtonDown(int button)
	{
		switch (button)
		{
		case 0: return m_leftMouseButtonPressed;
		case 1: return m_rightMouseButtonPressed;
		case 2: return m_middleMouseButtonPressed;
		default: return false;
		}
	}

	// Mouse button released this frame
	bool GetMouseButtonUp(int button)
	{
		switch (button)
		{
		case 0: return m_leftMouseButtonReleased;
		case 1: return m_rightMouseButtonReleased;
		case 2: return m_middleMouseButtonReleased;
		default: return false;
		}
	}

	// Get scroll delta (x = horizontal, y = vertical)
	glm::vec2 GetScrollDelta()
	{
		return m_scrollDelta;
	}

    bool GetKey(std::string key)
    {
        if (key == "jump")
        {
            return m_jump;
        }
        if (key == "leftshift")
        {
            return m_leftshift;
        }
		if (key == "D")
		{
			return m_pressedD;
		}
		if (key == "A")
		{
			return m_pressedA;
		}
    }

private:
    InputManager()
	{
		m_horizontal = 0;
		m_vertical = 0;
		m_leftMouseButton = false;
		m_rightMouseButton = false;
		m_middleMouseButton = false;
		m_leftMouseButtonPressed = false;
		m_rightMouseButtonPressed = false;
		m_middleMouseButtonPressed = false;
		m_leftMouseButtonReleased = false;
		m_rightMouseButtonReleased = false;
		m_middleMouseButtonReleased = false;
		m_scrollDelta = glm::vec2(0.0f, 0.0f);
	};
	InputManager(InputManager const&) = delete;
	void operator= (InputManager const&) = delete;

    float m_horizontal;
    float m_vertical;

    bool m_jump;
    bool m_leftshift;

	bool m_pressedD;
	bool m_holdD;
	bool m_pressedA;
	bool m_holdA;

    glm::vec2 m_mousePosition;

	// Mouse button states
	bool m_leftMouseButton;
	bool m_rightMouseButton;
	bool m_middleMouseButton;

	// Mouse button pressed this frame
	bool m_leftMouseButtonPressed;
	bool m_rightMouseButtonPressed;
	bool m_middleMouseButtonPressed;

	// Mouse button released this frame
	bool m_leftMouseButtonReleased;
	bool m_rightMouseButtonReleased;
	bool m_middleMouseButtonReleased;

	// Scroll wheel delta
	glm::vec2 m_scrollDelta;

	// Previous frame mouse button states for edge detection
	bool m_prevLeftMouseButton = false;
	bool m_prevRightMouseButton = false;
	bool m_prevMiddleMouseButton = false;

	void UpdateMouseButton(GLFWwindow* window, int glfwButton, bool& current, bool& pressed, bool& released)
	{
		bool prev = current;
		current = glfwGetMouseButton(window, glfwButton) == GLFW_PRESS;
		pressed = current && !prev;
		released = !current && prev;
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (manager)
		{
			// Callback is handled in Listen() for consistent frame-based updates
		}
	}

	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		if (manager)
		{
			manager->m_scrollDelta.x += static_cast<float>(xoffset);
			manager->m_scrollDelta.y += static_cast<float>(yoffset);
		}
	}

	static void KeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{

		}
	}
};
} // namespace core::managers

#endif  // SRC_INPUTMANAGER_H__