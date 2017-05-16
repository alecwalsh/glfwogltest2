cmake_minimum_required(VERSION 3.0.0)

project (glfwogltest2)

file(GLOB SOURCES "*.cpp")
 
add_executable(glfwogltest2 ${SOURCES})

target_include_directories(glfwogltest2 PUBLIC ./include)
target_link_libraries(glfwogltest2 GLEW GL glfw SOIL)
