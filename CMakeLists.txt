cmake_minimum_required(VERSION 3.1)
project(GLProj)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

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

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
		CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	
endif()

#linking and include dependencies
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(assimp)
add_subdirectory(bullet3)
add_subdirectory(cryptopp)
set(STB_SRCS stb/stb.h stb/stb_image.h stb/stb_dxt.h stb/stb_vorbis.c)

find_package(OpenGL REQUIRED)

#source project
add_subdirectory(src)

add_executable(GLProj ${SRCS} ${STB_SRCS})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
		CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(GLProj PUBLIC -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(GLProj PUBLIC /W4 /sdl)
endif()

target_link_libraries(GLProj glfw)
target_link_libraries(GLProj glm)
target_link_libraries(GLProj assimp)
#bullet3 libraries
target_link_libraries(GLProj Bullet3Collision)
target_link_libraries(GLProj Bullet3Common)
target_link_libraries(GLProj Bullet3Dynamics)
target_link_libraries(GLProj Bullet3Geometry)
target_link_libraries(GLProj Bullet3OpenCL_clew)
target_link_libraries(GLProj Bullet2FileLoader)
target_link_libraries(GLProj BulletCollision)
target_link_libraries(GLProj BulletDynamics)
target_link_libraries(GLProj BulletInverseDynamics)
target_link_libraries(GLProj BulletSoftBody)
target_link_libraries(GLProj LinearMath)
#
target_link_libraries(GLProj cryptopp-static)
target_link_libraries(GLProj ${OPENGL_LIBRARIES})
target_include_directories(GLProj PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(GLProj PUBLIC stb)
target_include_directories(GLProj PUBLIC glm/glm)
target_include_directories(GLProj PUBLIC assimp/include)
target_include_directories(GLProj PUBLIC bullet3/src)
target_include_directories(GLProj PUBLIC cryptopp)
target_include_directories(GLProj PUBLIC src/include)

add_custom_command(TARGET GLProj PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${CMAKE_CURRENT_SOURCE_DIR}/src/data" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/data")
add_custom_command(TARGET GLProj PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${CMAKE_CURRENT_SOURCE_DIR}/src/data" "${CMAKE_CURRENT_BINARY_DIR}/data")
