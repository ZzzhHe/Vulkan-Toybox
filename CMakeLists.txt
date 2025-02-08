# Project root CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(VulkanToys VERSION 1.0.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Vulkan REQUIRED COMPONENTS glslangValidator)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED) 

# 3rd party headers (STB, etc)
add_library(3rd_party INTERFACE)
target_include_directories(3rd_party INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd
)

# Modern compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Output directory configuration
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add common library and toys
add_subdirectory(common)
add_subdirectory(toys)