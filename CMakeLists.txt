cmake_minimum_required(VERSION 3.8.0)

project(glfwogltest2 VERSION 0.0.0.2)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "include/*.h")
set(SOURCES ${SOURCES} ${HEADERS} glad/glad.c imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_demo.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp)

find_package(Threads QUIET REQUIRED)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(glfwogltest2 ${SOURCES})

if(MSVC)
    target_compile_definitions(glfwogltest2 PRIVATE NOMINMAX)
    target_compile_options(glfwogltest2 PRIVATE "/W4")
    target_compile_options(glfwogltest2 PRIVATE "/Zc:__cplusplus")
else()
    target_compile_options(glfwogltest2 PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(glfwogltest2 PUBLIC c_std_11 cxx_std_17)

target_include_directories(glfwogltest2 PUBLIC "include" "." "imgui")

target_link_libraries(glfwogltest2 PRIVATE glm::glm glfw assimp::assimp)

if(UNIX)
    target_link_libraries(glfwogltest2 PRIVATE stdc++fs) # Need to link with stdc++fs for std::filesystem support on some older compilers
endif()

configure_file(include/version.h.in version.h)
target_include_directories(glfwogltest2 PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(glfwogltest2 PUBLIC DEBUG)
endif()
