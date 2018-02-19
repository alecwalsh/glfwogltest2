cmake_minimum_required(VERSION 3.1.0)

project(glfwogltest2 VERSION 0.0.0.1)

add_subdirectory(lua_cpp_bind)

file(GLOB SOURCES "*.cpp")
set(SOURCES ${SOURCES} glad/glad.c)

add_executable(glfwogltest2 ${SOURCES})

target_compile_features(glfwogltest2 PUBLIC cxx_std_14)
set_target_properties(glfwogltest2 PROPERTIES CXX_STANDARD_REQUIRED true)
set_target_properties(glfwogltest2 PROPERTIES CXX_EXTENSIONS false)

target_include_directories(glfwogltest2 PUBLIC "include" PUBLIC ".")
target_link_libraries(glfwogltest2 GL glfw assimp dl lua lua_cpp_bind)

configure_file(include/version.h.in version.h)
target_include_directories(glfwogltest2 PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(glfwogltest2 PUBLIC -DDEBUG)
endif()
