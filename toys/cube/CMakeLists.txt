add_vulkan_toy(cube
    main.cpp
    cube_app.cpp
    cube_app.h
)

# Compile shaders
# Shader compilation
if(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
    # Vertex shader
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cube.vert.spv
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        ARGS -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert -o ${CMAKE_CURRENT_BINARY_DIR}/cube.vert.spv
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert
        VERBATIM
    )

    # Fragment shader
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cube.frag.spv
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        ARGS -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag -o ${CMAKE_CURRENT_BINARY_DIR}/cube.frag.spv
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag
        VERBATIM
    )

    # Add shader files as dependencies
    target_sources(cube PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/cube.vert.spv
        ${CMAKE_CURRENT_BINARY_DIR}/cube.frag.spv
    )
else()
    message(FATAL_ERROR "glslangValidator not found!")
endif()

# Copy shaders to build directory
add_custom_command(TARGET cube POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:cube>/shaders/
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/cube.vert.spv
        ${CMAKE_CURRENT_BINARY_DIR}/cube.frag.spv
        $<TARGET_FILE_DIR:cube>/shaders/
)
