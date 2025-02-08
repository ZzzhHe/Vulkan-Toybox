
# add_vulkan_toy(triangle
#     main.cpp  # Add your actual source files here
# )

# # Example shader compilation (customize for your needs)
# if(Vulkan_glslangValidator_FOUND)
#     add_custom_command(
#         OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vert.spv
#         COMMAND ${Vulkan_glslangValidator} -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.glsl -o ${CMAKE_CURRENT_BINARY_DIR}/vert.spv
#         DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.glsl
#     )
#     target_sources(triangle PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/vert.spv)
# endif()
# ###