cmake_minimum_required(VERSION 3.1)

add_sources(main.cpp)
add_sources(include/Texture.hpp Texture.cpp)
add_sources(include/TextureManager.hpp TextureManager.cpp)
add_sources(include/gl_core_4_5.h gl_core_4_5.c)
add_sources(include/Sampler.hpp Sampler.cpp)
add_sources(include/MeshDataBuffer.hpp MeshDataBuffer.cpp)
add_sources(include/MeshIndexBuffer.hpp MeshIndexBuffer.cpp)
add_sources(include/MeshArrayBuffer.hpp MeshArrayBuffer.cpp)
add_sources(include/Mesh.hpp Mesh.cpp)
add_sources(include/Shader.hpp Shader.cpp)
add_sources(include/ShaderManager.hpp ShaderManager.cpp)
add_sources(include/ShadingProgram.hpp ShadingProgram.cpp)
add_sources(include/Model.hpp Model.cpp)
add_sources(include/MeshManager.hpp MeshManager.cpp)
add_sources(include/Material.hpp Material.cpp)
add_sources(include/LocalSharedPtr.hpp LocalSharedPtr.cpp)
add_sources(include/SceneGraph.hpp SceneGraph.cpp)
add_sources(include/Camera.hpp Camera.cpp)
add_sources(include/Transform.hpp Transform.cpp)
add_sources(include/RenderManager.hpp RenderManager.cpp)
add_sources(include/AssetManager.hpp AssetManager.cpp)

#file(COPY ./data/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
