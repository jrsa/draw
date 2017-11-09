cmake_minimum_required(VERSION 3.3)
set (CMAKE_CXX_STANDARD 11)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)
find_package(glog REQUIRED)

set(libs PUBLIC glbinding::glbinding glfw glog::glog)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# will be made into the library compilation target
set(shared_sources
    glfw_app.cpp
    shader.cpp
    particle_buffer.cpp
    fbo.cpp
    billboard.cpp
    )


# will go in demos/ folder or something
set(target point)
add_executable(${target} 
    ${target}.cpp
    ${shared_sources}
)
target_link_libraries(${target} ${libs})

set(target fbloop)
add_executable(${target} 
    ${target}.cpp
    ${shared_sources}
)
target_link_libraries(${target} ${libs})

set(target tfb)
add_executable(${target} 
    ${target}.cpp
    ${shared_sources}
)
target_link_libraries(${target} ${libs})