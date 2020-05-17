cmake_minimum_required(VERSION 3.3)
set (CMAKE_CXX_STANDARD 11)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)

# on arch, the glogConfig.cmake script is not packaged with glog :/
#find_package(glog REQUIRED)
set(GLOG_LIBRARY "/usr/lib/libglog.so")

set(libs PUBLIC glbinding::glbinding glfw ${GLOG_LIBRARY})

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

set(target box)
add_executable(${target} 
    ${target}.cpp
    glfw_app.cpp
    shader.cpp
)
target_link_libraries(${target} ${libs})

set(target q)
add_executable(${target}
    ${target}.cpp
    billboard.cpp
    glfw_app.cpp
    shader.cpp
)
target_link_libraries(${target} ${libs})
