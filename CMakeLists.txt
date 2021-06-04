cmake_minimum_required(VERSION 3.8.0)

project(glfwogltest2 VERSION 0.0.0.2)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "include/*.hpp")
file(GLOB IMGUI_SOURCES "external/imgui/*.cpp")
set(SOURCES ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} external/glad/glad.c)

find_package(Threads QUIET REQUIRED)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(glfwogltest2 ${SOURCES})

add_subdirectory(Physics)

if(MSVC)
    target_compile_definitions(glfwogltest2 PRIVATE NOMINMAX)
    target_compile_options(glfwogltest2 PRIVATE "/W4")
    target_compile_options(glfwogltest2 PRIVATE "/Zc:__cplusplus") # Use the correct value for the __cplusplus macro
else()
    target_compile_options(glfwogltest2 PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(glfwogltest2 PUBLIC c_std_11 cxx_std_17)

target_include_directories(glfwogltest2 PUBLIC "include" "external")

target_link_libraries(glfwogltest2 PRIVATE glm::glm glfw assimp::assimp glfwogltest2_physics)

if(UNIX)
    target_link_libraries(glfwogltest2 PRIVATE stdc++fs) # Need to link with stdc++fs for std::filesystem support on some older compilers
endif()

configure_file(include/version.hpp.in include/version.hpp)
target_include_directories(glfwogltest2 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(glfwogltest2 PUBLIC DEBUG)
endif()
