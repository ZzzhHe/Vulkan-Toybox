function(add_vulkan_toy name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE vulkan_common)
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/toys/${name}
    )
endfunction()

add_subdirectory(triangle)
add_subdirectory(cube)