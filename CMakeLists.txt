cmake_minimum_required(VERSION 3.1)
project(GLProj)

set(executable_name ${PROJECT_NAME})

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
add_subdirectory(json)
add_subdirectory(stb)
add_subdirectory(sqlite)

find_package(OpenGL REQUIRED)

#source project
add_subdirectory(src)

add_executable(${executable_name} ${SRCS})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
		CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${executable_name} PUBLIC -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(${executable_name} PUBLIC /W3 /sdl)
endif()

target_link_libraries(${executable_name} glfw)
target_link_libraries(${executable_name} glm)
target_link_libraries(${executable_name} assimp)
target_link_libraries(${executable_name} stb)
target_link_libraries(${executable_name} sqlite)
#bullet3 libraries
target_link_libraries(${executable_name} Bullet3Collision Bullet3Common Bullet3Dynamics 
Bullet3Geometry Bullet3OpenCL_clew Bullet2FileLoader BulletCollision BulletDynamics
BulletInverseDynamics BulletSoftBody LinearMath)
#
target_link_libraries(${executable_name} nlohmann_json)
target_link_libraries(${executable_name} zlibstatic)
target_link_libraries(${executable_name} ${OPENGL_LIBRARIES})
target_include_directories(${executable_name} PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${executable_name} PUBLIC stb)
target_include_directories(${executable_name} PUBLIC glm/glm)
target_include_directories(${executable_name} PUBLIC assimp/include)
target_include_directories(${executable_name} PUBLIC bullet3/src)
target_include_directories(${executable_name} PUBLIC json/src)
target_include_directories(${executable_name} PUBLIC sqlite)
target_include_directories(${executable_name} PUBLIC assimp/contrib/zlib)
target_include_directories(${executable_name} PUBLIC src/include)

add_custom_command(TARGET ${executable_name} PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${CMAKE_CURRENT_SOURCE_DIR}/src/data" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/data")
add_custom_command(TARGET ${executable_name} PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${CMAKE_CURRENT_SOURCE_DIR}/src/data" "${CMAKE_CURRENT_BINARY_DIR}/data")
