cmake_minimum_required(VERSION 3.3)

# hacks (should figure out how to do properly/why it doesnt work)
set(GLFW3_DIR "/usr/local/lib/cmake/glfw/")
set(assimp_DIR "/usr/local/Cellar/assimp/3.3.1/lib/cmake/")
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
# end hacks

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(assimp REQUIRED)

set(GLM_INCLUDE_DIR "/usr/local/include/glm")
set(GLOG_LIBRARY "/usr/local/opt/glog/lib/libglog.dylib")

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIR}
    ${GLBINDING_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    /usr/local/Cellar/stk/4.5.0/include/stk/
    /usr/local/Cellar/liblo/0.28/include/
    /Users/jrsa/Downloads/OculusSDK/LibOVR/Include
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(libs
    ${OPENGL_LIBRARIES}
    ${GLFW3_LIBRARY}
    ${GLBINDING_LIBRARY}
    ${GLOG_LIBRARY}
        /usr/local/Cellar/assimp/3.3.1/lib/libassimp.3.3.1.dylib
#    /opt/rtaudio/librtaudio.dylib
#    /usr/local/Cellar/stk/4.5.0/lib/libstk.a
#    "/Library/Frameworks/Syphon.framework"
#    /usr/local/Cellar/liblo/0.28/lib/liblo.dylib
    /Users/jrsa/Downloads/OculusSDK/LibOVR/Lib/Mac/Debug/libovr.a
    /System/Library/Frameworks/Cocoa.framework
    /System/Library/Frameworks/IOKit.framework
)

set(sources
    glfw_app.cpp
    shader.cpp
    billboard.cpp
    fbo.cpp
    simple_file.cpp
#    syphon_server.mm
    hmd_dist_mesh.cpp
    particle_buffer.cpp
)

add_executable(meshtest glfw_app.cpp shader.cpp meshtest.cpp)
target_link_libraries(meshtest ${libs})
target_compile_options(meshtest PRIVATE ${DEFAULT_COMPILE_FLAGS})

set(target xformfbtest)

add_executable(${target}
        # xformfbtest.cpp
        # particle_buffer.cpp
        # shader.cpp
        glfw_app.cpp
        # simple_file.cpp
        xformfbnew.cpp
        )
target_link_libraries(${target} ${libs})
target_compile_options(${target} PRIVATE ${DEFAULT_COMPILE_FLAGS})
set_target_properties(${target}
        PROPERTIES
        LINKER_LANGUAGE              CXX
        FOLDER                      "${IDE_FOLDER}"
        COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
        COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
        LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
        LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"
        DEBUG_POSTFIX               "d${DEBUG_POSTFIX}")


set(target fbloop)

add_executable(${target}
        fbloop.cpp
        fbo.cpp
        billboard.cpp
        shader.cpp
        glfw_app.cpp
        simple_file.cpp
        )
target_link_libraries(${target} ${libs})
target_compile_options(${target} PRIVATE ${DEFAULT_COMPILE_FLAGS})
set_target_properties(${target}
        PROPERTIES
        LINKER_LANGUAGE              CXX
        FOLDER                      "${IDE_FOLDER}"
        COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
        COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
        LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
        LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"
        DEBUG_POSTFIX               "d${DEBUG_POSTFIX}")

add_executable(imptest imp.cpp)
target_link_libraries(imptest ${libs})

add_executable(conftest conf/read.cc)
target_link_libraries(conftest /usr/local/lib/libyaml.a)

