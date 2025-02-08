#ifndef SURFACE_H
#define SURFACE_H

#include <stdexcept>
#include <memory>
#include <vulkan/vulkan.h>

namespace vkcommon
{
  class Instance;
  class Window;

  class Surface
  {
  public:
    Surface(const Instance &instance, const Window &window);
    ~Surface();

    VkSurfaceKHR handle() const { return m_surface; }

  private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    const Instance &m_instanceRef;
  };

} // namespace vkcommon

#endif // SURFACE_H