#include "window.h"

namespace vkcommon
{
    Window::Window()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(kWidth, kHeight, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::pollEvents() const
    {
        glfwPollEvents();
    }

    void Window::processInput() {
        updateDeltaTime();

        if (m_keyCallback) {
            // Check for key states
            for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
                int state = glfwGetKey(m_window, key);
                if (state == GLFW_PRESS || state == GLFW_RELEASE) {
                    m_keyCallback(key, state);
                }
            }
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        self->m_framebufferResized = true;
    }

    void Window::updateDeltaTime() {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;
    }
} // namespace vkcommon