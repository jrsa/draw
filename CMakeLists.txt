cmake_minimum_required(VERSION 3.12)
project(draw)
set (CMAKE_CXX_STANDARD 11)

# for clang LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)
find_package(glog REQUIRED)

set(libs PUBLIC glbinding::glbinding glbinding::glbinding-aux glfw glog::glog)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# will be made into the library compilation target
set(shared_sources
    glfw_app.cpp
    shader.cpp
    particle_buffer.cpp
    fbo.cpp
    billboard.cpp
)

add_library(libdraw SHARED ${shared_sources})
target_link_libraries(libdraw ${libs})


set(target point)
add_executable(${target} 
    ${target}.cpp
)
target_link_libraries(${target} libdraw)

set(target fbloop)
add_executable(${target} 
    ${target}.cpp
)
target_link_libraries(${target} libdraw)

set(target tfb)
add_executable(${target} 
    ${target}.cpp
)
target_link_libraries(${target} libdraw)

set(target box)
add_executable(${target} 
    ${target}.cpp
)
target_link_libraries(${target} libdraw)

set(target q)
add_executable(${target}
    ${target}.cpp
)
target_link_libraries(${target} libdraw)

