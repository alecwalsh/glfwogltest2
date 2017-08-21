cmake_minimum_required(VERSION 3.1.0)

project(glfwogltest2)

file(GLOB SOURCES "*.cpp")
set(SOURCES ${SOURCES} glad/glad.c)
 
add_executable(glfwogltest2 ${SOURCES})

target_compile_features(glfwogltest2 PUBLIC cxx_std_14)
set_target_properties(glfwogltest2 PROPERTIES CXX_STANDARD_REQUIRED true)

target_link_libraries(glfwogltest2 GL glfw SOIL assimp dl lua)

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(glfwogltest2 PUBLIC -DDEBUG)
endif()
