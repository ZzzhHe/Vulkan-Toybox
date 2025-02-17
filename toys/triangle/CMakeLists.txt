add_vulkan_toy(triangle
    main.cpp
    triangle_app.cpp
    triangle_app.h
)

# Compile shaders
# Shader compilation
if(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
    # Vertex shader
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/triangle.vert.spv
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        ARGS -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.vert -o ${CMAKE_CURRENT_BINARY_DIR}/triangle.vert.spv
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.vert
        VERBATIM
    )

    # Fragment shader
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/triangle.frag.spv
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        ARGS -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.frag -o ${CMAKE_CURRENT_BINARY_DIR}/triangle.frag.spv
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.frag
        VERBATIM
    )

    # Add shader files as dependencies
    target_sources(triangle PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/triangle.vert.spv
        ${CMAKE_CURRENT_BINARY_DIR}/triangle.frag.spv
    )
else()
    message(FATAL_ERROR "glslangValidator not found!")
endif()

# Copy shaders to build directory
add_custom_command(TARGET triangle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:triangle>/shaders/
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/triangle.vert.spv
        ${CMAKE_CURRENT_BINARY_DIR}/triangle.frag.spv
        $<TARGET_FILE_DIR:triangle>/shaders/
)
