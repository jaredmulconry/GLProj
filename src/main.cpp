#include "GLFW/glfw3.h"
#include "assimp\Importer.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <thread>

using GlProj::Graphics::GetTextureManager;

int main()
try
{
	if (glfwInit() == GLFW_FALSE)
	{
		return EXIT_FAILURE;
	}

	auto win = glfwCreateWindow(640, 480, "Bad Window", nullptr, nullptr);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwMakeContextCurrent(win);

	Assimp::Importer importer;
	auto bunny = importer.ReadFile("./data/models/bunny.obj", 0);
	if (bunny == nullptr)
	{
		std::string err;
		err += importer.GetErrorString();
		throw std::runtime_error(err);
	}

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	while (!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(win);

		glfwPollEvents();
	}

	glfwDestroyWindow(win);

	glfwTerminate();
}
catch(std::exception& e)
{
	std::cout << e.what() << std::endl;
	throw;
}