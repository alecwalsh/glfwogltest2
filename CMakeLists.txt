cmake_minimum_required(VERSION 3.0.0)

project (glfwogltest2)

SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_STANDARD 14)

#Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

file(GLOB SOURCES "*.cpp")
set(SOURCES ${SOURCES} glad/glad.c)
 
add_executable(glfwogltest2 ${SOURCES})

target_link_libraries(glfwogltest2 GL glfw SOIL assimp dl)
