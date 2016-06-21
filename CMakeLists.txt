cmake_minimum_required(VERSION 3.3)
project(GLProj)

macro (add_sources)
    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND SRCS "${_relPath}/${_src}")
        else()
            list (APPEND SRCS "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate SRCS to parent directory
        set (SRCS ${SRCS} PARENT_SCOPE)
    endif()
endmacro()

#linking and include dependencies
add_subdirectory(glfw)
add_subdirectory(assimp)
include_directories(stb)
set(STB_SRCS stb.h stb_image.h stb_dxt.h stb_vorbis.c)

find_package(OpenGL REQUIRED)

#source project
add_subdirectory(src)

add_executable(GLProj ${SRCS} ${STB_SRCS})

target_link_libraries(GLProj glfw)
target_link_libraries(GLProj Assimp)
target_link_libraries(GLProj ${OPENGL_LIBRARIES})
target_include_directories(GLProj PUBLIC ${OPENGL_INCLUDE_DIR})