#include "gl_core_4_1.h"
#include "GLFW/glfw3.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Material.hpp"
#include "MeshManager.hpp"
#include "Model.hpp"
#include "RenderManager.hpp"
#include "SceneGraph.hpp"
#include "Shader.hpp"
#include "ShaderManager.hpp"
#include "ShadingProgram.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include "Mesh.hpp"
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace GlProj::Graphics;

void glfwExecErrorCallback(int, const char* msg)
{
	std::cerr << "glfw error: " << msg << std::endl;
}

LocalSharedPtr<Material> GetDefaultMaterial()
{
	static bool firstRun = true;
	static auto prog = GenerateProgram();
	static auto mat = GlProj::Utilities::make_localshared<Material>();
	if (!firstRun) return mat;

	auto vs = LoadShader(GetShaderManager(), GL_VERTEX_SHADER, "./data/shaders/BasicShader.vs");
	auto fs = LoadShader(GetShaderManager(), GL_FRAGMENT_SHADER, "./data/shaders/BasicShader.fs");
	
	AttachShader(prog.get(), vs.get());
	AttachShader(prog.get(), fs.get());
	LinkProgram(prog.get());
	prog->FetchProgramInfo();

	*mat = prog;
	firstRun = false;

	return mat; 
}

void PrepareAndRunGame(GLFWwindow* window)
{
	Assimp::Importer importer{};
	auto bunny = importer.ReadFile("./data/models/bunny.obj", aiProcessPreset_TargetRealtime_Quality);
	if (bunny == nullptr)
	{
		std::string err;
		err += importer.GetErrorString();
		throw std::runtime_error(err);
	}
	std::vector<Renderable> submeshes; 
	submeshes.reserve(bunny->mNumMeshes);
	auto material = GetDefaultMaterial();

	for (int i = 0; i < int(bunny->mNumMeshes); ++i)
	{
		submeshes.push_back({ RegisterMesh(GetMeshManager(), bunny->mMeshes[i], bunny->mMeshes[i]->mName.C_Str()), 
							  nullptr });
	}

	SceneGraph<ModelData> bunnyGraph;

	//Model bunyModel{ submeshes }

	GlProj::Utilities::TestSceneGraph();

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	auto renderer = GetRenderManager();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		Draw(renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main()
try
{
	glfwSetErrorCallback(glfwExecErrorCallback);
	if (glfwInit() == GLFW_FALSE)
	{
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	auto win = glfwCreateWindow(1280, 720, "Bad Window", nullptr, nullptr);
	
	if (win == nullptr)
	{
		return EXIT_FAILURE;
	}

	int contextMajorVersion = glfwGetWindowAttrib(win, GLFW_CONTEXT_VERSION_MAJOR);
	int contextMinorVersion = glfwGetWindowAttrib(win, GLFW_CONTEXT_VERSION_MINOR);
	
	std::cout << "Major version is " << contextMajorVersion << '\n';
	std::cout << "Minor version is " << contextMinorVersion << std::endl;

	glfwMakeContextCurrent(win);

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED)
	{
		return EXIT_FAILURE;
	}

	PrepareAndRunGame(win);

	glfwDestroyWindow(win);
	glfwTerminate();
}
catch(std::exception& e)
{
	std::cerr << e.what() << std::endl;
	throw;
}
