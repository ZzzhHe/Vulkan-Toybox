function(add_vulkan_toy name)
    # Create executable
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE vulkan_common)
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/toys/${name}
    )

    if(NOT Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
        message(FATAL_ERROR "glslangValidator not found!")
    endif()

    # Compile shaders
    set(shader_types "vert" "frag" "geom")
    set(spv_files "")
    
    foreach(shader_type ${shader_types})
        set(shader_file "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${name}.${shader_type}")
        if(EXISTS ${shader_file})
            compile_shader(${name} ${shader_file} ${shader_type})
            list(APPEND spv_files ${spv_file})
        endif()
    endforeach()

endfunction()

# function to compile shader
function(compile_shader target shader_file shader_type)
    get_filename_component(shader_name ${shader_file} NAME)
    # Change the output path to match the executable directory
    set(spv_file "${CMAKE_BINARY_DIR}/bin/toys/${target}/shaders/${shader_name}.spv")
    
    add_custom_command(
        OUTPUT ${spv_file}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/toys/${target}"
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        ARGS -V ${shader_file} -o ${spv_file}
        DEPENDS ${shader_file}
        VERBATIM
    )
    
    target_sources(${target} PRIVATE ${spv_file})
    return(PROPAGATE spv_file)
endfunction()

add_subdirectory(triangle)
add_subdirectory(cube)
# add_subdirectory(explosion)