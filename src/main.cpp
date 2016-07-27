#include "gl_core_4_1.h"
#include "GLFW/glfw3.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\scene.h"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include "Mesh.hpp"
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>

using namespace GlProj::Graphics;

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

	GlProj::Graphics::Mesh bunnyMesh{ *bunny->mMeshes };

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

int main()
try
{
	if (glfwInit() == GLFW_FALSE)
	{
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	auto win = glfwCreateWindow(1280, 720, "Bad Window", nullptr, nullptr);
	
	if (win == nullptr)
	{
		return EXIT_FAILURE;
	}

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