# Smart source discovery with controlled globbing
set(COMMON_SOURCE_DIRS
    core
    graphics 
    resources
    sync
)

# Collect sources from specified directories
file(GLOB_RECURSE COMMON_SOURCES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_SOURCE_DIRS}/*.cpp
)

add_library(vulkan_common STATIC ${COMMON_SOURCES})

# Interface configuration
target_include_directories(vulkan_common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(vulkan_common PUBLIC
    Vulkan::Vulkan
    glfw
    glm::glm
    3rd_party  # Inherits STB headers
)

target_compile_features(vulkan_common PUBLIC cxx_std_20)

# Optional: Validate source collection
if(NOT COMMON_SOURCES)
    message(FATAL_ERROR "No source files found in common library directories!")
endif()