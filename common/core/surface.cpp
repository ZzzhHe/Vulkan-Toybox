#include "surface.h"

#include "core/instance.h"
#include "core/window.h"

namespace vkcommon
{
  Surface::Surface(const Instance &instance, const Window &window) : m_instanceRef(instance)
  {
    if (glfwCreateWindowSurface(m_instanceRef.handle(), window.window(), nullptr, &m_surface) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create window surface!");
    }
  }

  Surface::~Surface()
  {
    vkDestroySurfaceKHR(m_instanceRef.handle(), m_surface, nullptr);
  }
}