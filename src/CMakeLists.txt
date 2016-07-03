cmake_minimum_required(VERSION 3.1)

add_sources(main.cpp)
add_sources(include/Texture.hpp Texture.cpp)
add_sources(include/TextureManager.hpp TextureManager.cpp)
add_sources(include/OpenGLTypes.hpp OpenGLTypes.cpp)
add_sources(include/gl_core_4_1.h gl_core_4_1.c)
add_sources(include/Sampler.hpp Sampler.cpp)
add_sources(include/MeshDataBuffer.hpp MeshDataBuffer.cpp)
add_sources(include/Mesh.hpp Mesh.cpp)
add_sources(include/MeshIndexBuffer.hpp MeshIndexBuffer.cpp)

#file(COPY ./data/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
