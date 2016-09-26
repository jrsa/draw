cmake_minimum_required(VERSION 3.3)
set(target gltest)

# hacks (should figure out how to do properly/why it doesnt work)
set(GLFW3_DIR "/usr/local/lib/cmake/glfw/")
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
# end hacks

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glbinding REQUIRED)

set(GLM_INCLUDE_DIR "/usr/local/include/glm")
set(GLOG_LIBRARY "/usr/local/opt/glog/lib/libglog.dylib")

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIR}
    ${GLBINDING_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    /usr/local/Cellar/stk/4.5.0/include/stk/
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(libs
    ${OPENGL_LIBRARIES}
    ${GLFW3_LIBRARY}
    ${GLBINDING_LIBRARY}
    ${GLOG_LIBRARY}
    /opt/rtaudio/librtaudio.dylib
    /usr/local/Cellar/stk/4.5.0/lib/libstk.a
#    "/Library/Frameworks/Syphon.framework"
)

set(sources

    glfw_app.cpp
    shader.cpp
    billboard.cpp
    fbo.cpp
    syphon_server.mm
    )

add_executable(${target} ${sources} main.cpp)
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
