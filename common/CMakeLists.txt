# Create empty list for sources
set(COMMON_SOURCE_DIRS
    core
    graphics 
    resources
    sync
)

# Collect sources from each directory and subdirectories
set(COMMON_SOURCES "")
foreach(DIR ${COMMON_SOURCE_DIRS})
    file(GLOB_RECURSE DIR_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.cpp"
    )
    list(APPEND COMMON_SOURCES ${DIR_SOURCES})
endforeach()

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
    assimp::assimp
    3rd_party  # This now correctly includes stb_image.h
)

target_compile_features(vulkan_common PUBLIC cxx_std_20)

# Print collected sources for debugging
message(STATUS "Common library sources:")
foreach(SOURCE ${COMMON_SOURCES})
    message(STATUS "  ${SOURCE}")
endforeach()

# Validate source collection
if(NOT COMMON_SOURCES)
    message(FATAL_ERROR "No source files found in common library directories!")
endif()