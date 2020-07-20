cmake_minimum_required(VERSION 3.3)
set (CMAKE_CXX_STANDARD 11)

# rebasing these in... not sure how necessary
set(CMAKE_BUILD_TYPE "Debug")
set(glbinding_DIR "/usr/local/Cellar/glbinding/2.1.4/")

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)

# on arch, the glogConfig.cmake script is not packaged with glog :/
# set(GLOG_LIBRARY "/usr/lib/libglog.so")
# set(libs PUBLIC glbinding::glbinding glfw ${GLOG_LIBRARY})

find_package(glog REQUIRED)
set(libs PUBLIC glbinding::glbinding glfw glog::glog
    # if i wasnt a lazy asshole i would just write a cmake script to pull this in ._.
    /Users/jrsa/Desktop/OculusSDK/LibOVR/Lib/Mac/Release/libovr.a
    /System/Library/Frameworks/Cocoa.framework
    /System/Library/Frameworks/IOKit.framework
    )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    /usr/local/include
    # ditto for this, lazy asshole
    /Users/jrsa/Desktop/OculusSDK/LibOVR/Include
    )

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


set(target meshtest)
add_executable(${target}
    ${target}.cpp
    billboard.cpp
    glfw_app.cpp
    shader.cpp
    fbo.cpp
)
target_link_libraries(${target} ${libs})
