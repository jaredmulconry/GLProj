cmake_minimum_required(VERSION 3.1)
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
set(STB_SRCS stb/stb.h stb/stb_image.h stb/stb_dxt.h stb/stb_vorbis.c)

find_package(OpenGL REQUIRED)

#source project
add_subdirectory(src)

add_executable(GLProj ${SRCS} ${STB_SRCS})

target_link_libraries(GLProj glfw)
target_link_libraries(GLProj assimp)
target_link_libraries(GLProj ${OPENGL_LIBRARIES})
target_include_directories(GLProj PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(GLProj PUBLIC stb)
target_include_directories(GLProj PUBLIC assimp/include)
target_include_directories(GLProj PUBLIC src/include)

add_custom_command(TARGET GLProj PRE_BUILD
										COMMAND ${CMAKE_COMMAND} -E copy_directory
										"${CMAKE_CURRENT_SOURCE_DIR}/src/data" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/data")