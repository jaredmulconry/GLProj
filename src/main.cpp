#include "GLFW/glfw3.h"
#include <chrono>
#include <cstdlib>
#include <thread>

int main()
{
	if (glfwInit() == GLFW_FALSE)
	{
		return EXIT_FAILURE;
	}

	auto win = glfwCreateWindow(320, 200, "Bad Window", nullptr, nullptr);

	std::this_thread::sleep_for(std::chrono::seconds(2));

	glfwDestroyWindow(win);

	glfwTerminate();
}