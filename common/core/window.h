#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

#include <functional>

namespace vkcommon
{

    const uint32_t kWidth = 800;
    const uint32_t kHeight = 600;

    class Window
    {
    public:
        Window();
        ~Window();

        // Delete copy operations
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool shouldClose() const;
        void pollEvents() const;

        // Generic input handling
        using KeyCallback = std::function<void(int key, int action)>;
        void setKeyCallback(KeyCallback callback) { m_keyCallback = callback; }
        void processInput();

        // Window state
        GLFWwindow* window() const { return m_window; }
        bool isFramebufferResized() const { return m_framebufferResized; }
        void resetFramebufferResized() { m_framebufferResized = false; }

        float getDeltaTime() const { return m_deltaTime; }

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

    private:
        static void framebufferResizeCallback(GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height);
        void updateDeltaTime();

        GLFWwindow* m_window;
        bool m_framebufferResized = false;
        KeyCallback m_keyCallback;

        float m_deltaTime = 0.0f;
        float m_lastFrame = 0.0f;
    };

} // namespace vkcommon

#endif // WINDOW_H