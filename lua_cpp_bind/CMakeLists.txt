cmake_minimum_required(VERSION 3.1.0)

project(lua_cpp_bind)

file(GLOB SOURCES "*.cpp")
 
add_library(lua_cpp_bind STATIC ${SOURCES})

target_compile_features(lua_cpp_bind PUBLIC cxx_std_11)
set_target_properties(lua_cpp_bind PROPERTIES CXX_STANDARD_REQUIRED true)

target_link_libraries(lua_cpp_bind lua)

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(lua_cpp_bind PUBLIC -DDEBUG)
endif()
