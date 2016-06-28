cmake_minimum_required(VERSION 3.1)

add_sources(main.cpp)
add_sources(include/Texture.hpp Texture.cpp)
add_sources(include/TextureManager.hpp TextureManager.cpp)
add_sources(include/OpenGLTypes.hpp OpenGLTypes.cpp)
add_sources(include/gl_core_4_1.h gl_core_4_1.c)
add_sources(include/Sampler.hpp Sampler.cpp)

#file(COPY ./data/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
