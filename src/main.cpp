#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

using namespace GlProj::Graphics;

inline void APIENTRY GLDbgCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei, const GLchar* message,
	const void*)
{
	thread_local static std::string output;
	output.clear();
	output += "GL Log:\nSource: ";

	switch (source)
	{
	default:
		output += "GL_DEBUG_SOURCE_OTHER";
		break;
	case GL_DEBUG_SOURCE_API:
		output += "GL_DEBUG_SOURCE_API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		output += "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		output += "GL_DEBUG_SOURCE_SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		output += "GL_DEBUG_SOURCE_THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		output += "GL_DEBUG_SOURCE_APPLICATION";
		break;
	}
	output += "\nType: ";

	auto oStream = &std::cout;

	switch (type)
	{
	default:
		output += "GL_DEBUG_TYPE_OTHER";
		break;
	case GL_DEBUG_TYPE_ERROR:
		output += "GL_DEBUG_TYPE_ERROR";
		oStream = &std::cerr;
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		output += "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		oStream = &std::cerr;
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		output += "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		oStream = &std::cerr;
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		output += "GL_DEBUG_TYPE_PORTABILITY";
		oStream = &std::cerr;
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		output += "GL_DEBUG_TYPE_PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_MARKER:
		output += "GL_DEBUG_TYPE_MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		output += "GL_DEBUG_TYPE_PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		output += "GL_DEBUG_TYPE_POP_GROUP";
		break;
	}
	output += "\nID: ";
	output += std::to_string(id);
	output += "\nSeverity: ";

	switch (severity)
	{
	default:
		output += "GL_DEBUG_SEVERITY_NOTIFICATION";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		output += "GL_DEBUG_SEVERITY_HIGH";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		output += "GL_DEBUG_SEVERITY_MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		output += "GL_DEBUG_SEVERITY_LOW";
		break;
	}
	output += "\nMessage:\n";
	output += message;

	(*oStream) << output << "\n\n";
}

inline glm::mat4 aiToGlm(const aiMatrix4x4& o)
{
	return{ o.a1, o.a2, o.a3, o.a4,
		   o.b1, o.b2, o.b3, o.b4,
		   o.c1, o.c2, o.c3, o.c4,
		   o.d1, o.d2, o.d3, o.d4 };
}

template<typename I>
//requires value_type of iterator is std::string
inline void MakeNamesUnique(I first, I last)
{
	auto firstMesh = first;
	while (firstMesh != last)
	{
		auto pairStart = std::adjacent_find(firstMesh, last);
		if (pairStart == last)
		{
			break;
		}
		int i = 0;

		std::string prev = *pairStart;
		first = pairStart;
		//First duplicate will have a 0-suffix
		*first += std::to_string(i);
		++i;
		++pairStart;
		//Second duplicate will have a 1-suffix
		*pairStart += std::to_string(i);
		++pairStart;
		++i;
		//Subsequent duplicates will have
		while (pairStart != last && *pairStart == prev)
		{
			*pairStart += std::to_string(i);
			++pairStart;
			++i;
		}
		firstMesh = pairStart;
	}
}

inline glm::mat4 ApplyHierarchy(const GlProj::Utilities::SceneNode<ModelData>& n)
{
	auto node = &n;

	auto t = glm::mat4(1);
	while (node != nullptr)
	{
		t *= node->data.transform;
		node = node->parent;
	}

	return t;
}

void AddChildren(SceneGraph<ModelData>& graph,
	GlProj::Utilities::SceneNode<ModelData>* parent,
	aiNode* node)
{
	for (int i = 0; i < int(node->mNumChildren); ++i)
	{
		graph.emplace(parent, aiToGlm(node->mChildren[i]->mTransformation),
			std::vector<unsigned int>(node->mChildren[i]->mMeshes, node->mChildren[i]->mMeshes + node->mChildren[i]->mNumMeshes),
			node->mChildren[i]->mName.C_Str());
	}

	for (int i = 0; i < int(node->mNumChildren); ++i)
	{
		AddChildren(graph, parent->children->data() + i, node->mChildren[i]);
	}
}

void PopulateGraph(SceneGraph<ModelData>& graph, aiNode* root)
{
	auto parent = graph.emplace(nullptr, aiToGlm(root->mTransformation),
		std::vector<unsigned int>(root->mMeshes, root->mMeshes + root->mNumMeshes),
		root->mName.C_Str());

	AddChildren(graph, parent, root);
}

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
	Model model;
	std::vector<local_shared_ptr<RenderableHandle>> handles;
	auto renderer = GetRenderManager();
	auto batch = GenerateRenderBatch(renderer);

	{
		Assimp::Importer importer{};
#ifdef _DEBUG
		Assimp::DefaultLogger::create("AssimpLog.txt", Assimp::Logger::VERBOSE, aiDefaultLogStream_STDOUT);
#endif
		auto bunny = importer.ReadFile("./data/models/armadillo.obj", aiProcess_Triangulate | aiProcess_SortByPType
			| aiProcess_GenUVCoords | aiProcess_OptimizeGraph
			| aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals);
		importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 0xffff);
		bunny = importer.ApplyPostProcessing(aiProcess_SplitLargeMeshes | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure);

		if (bunny == nullptr)
		{
			std::string err;
			err += importer.GetErrorString();
			throw std::runtime_error(err);
		}

		Assimp::DefaultLogger::kill();

		const auto& initText = "System Init";

		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, sizeof(initText), initText);
		std::vector<Renderable> submeshes;
		submeshes.reserve(bunny->mNumMeshes);
		auto material = GetDefaultMaterial();

		std::vector<std::string> meshNames;
		meshNames.reserve(bunny->mNumMeshes);
		std::transform(bunny->mMeshes, bunny->mMeshes + bunny->mNumMeshes, std::back_inserter(meshNames),
			[](const auto& x)
		{
			return x->mName.C_Str();
		});

		MakeNamesUnique(meshNames.begin(), meshNames.end());

		for (int i = 0; i < int(bunny->mNumMeshes); ++i)
		{
			submeshes.push_back({ RegisterMesh(GetMeshManager(), bunny->mMeshes[i], meshNames[i]),
								  nullptr });
		}

		SceneGraph<ModelData> bunnyGraph;
		PopulateGraph(bunnyGraph, bunny->mRootNode);

		model = Model{ submeshes, std::move(bunnyGraph) };

		GlProj::Utilities::TestSceneGraph();

		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

		for (int i = 0; i < int(submeshes.size()); ++i)
		{
			handles.push_back(SubmitRenderable(batch.get(), *(submeshes[i].mesh), submeshes[i].material.get()));
		}
		auto& hierarchy = model.GetHierarchy();
		for (auto pos = hierarchy.begin(); pos != hierarchy.end(); ++pos)
		{
			if (pos->meshes.empty()) continue;

			auto transform = ApplyHierarchy(*pos.current);
			auto& dat = *pos;
			for (const auto& i : dat.meshes)
			{
				SetTransform(handles[i].get(), transform);
			}
		}
		SetOverrideMaterial(batch.get(), material.get());
		auto cam = Camera{ Camera::Orthographic{glm::vec2{8.0f, 4.5f}}, -5.0f, 5.0f };
		cam.transform = Transform{ {0.0f, -1.0f, 0.0f}, glm::quat(), {1.0f, 1.0f, 1.0f} };
		UpdateBatchCamera(batch.get(), cam);
	}

	glPopDebugGroup();

	const auto& renderingGroup = "Render loop";

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, sizeof(renderingGroup), renderingGroup);


	auto& hierarchy = model.GetHierarchy();
	float angle = 0.0f;
	const float rotationSpeed = 0.2f;

	auto prevTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

		auto newTime = glfwGetTime();
		auto delta = newTime - prevTime;
		prevTime = newTime;
		angle += float(delta) * rotationSpeed;

		auto rootTransform = glm::rotate(glm::mat4(1), angle, glm::vec3{ 0.0f, 1.0f, 0.0f });

		for (auto pos = hierarchy.begin(); pos != hierarchy.end(); ++pos)
		{
			if (pos->meshes.empty()) continue;

			auto transform = ApplyHierarchy(*pos.current);
			auto& dat = *pos;
			for (const auto& i : dat.meshes)
			{
				SetTransform(handles[i].get(), rootTransform * transform);
			}
		}


		Draw(renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glPopDebugGroup();
}

void LogExceptions()
{
	auto exPtr = std::current_exception();
	int depth = 0;
	static const constexpr int tabwidth = 2;

	while(true)
	{
		try
		{
			std::rethrow_exception(exPtr);
		}
		catch (std::nested_exception& e)
		{
			auto& firstExcept = dynamic_cast<std::exception&>(e);
			std::string msg = firstExcept.what();
			std::cerr << std::right << std::setw(msg.length() + tabwidth * depth) << msg << std::endl;
			exPtr = e.nested_ptr();
			++depth;
		}
		catch (std::exception& e)
		{
			std::string msg = e.what();
			std::cerr << std::right << std::setw(msg.length() + tabwidth * depth) << msg << std::endl;
			break;
		}
		catch (...)
		{
			std::string msg = "Unknown exception thrown.";
			std::cerr << std::right << std::setw(msg.length() + tabwidth * depth) << msg << std::endl;
			break;
		}
	}
}

GLFWwindow* CreateWindowOnMonitor(const char* name, GLFWmonitor* mon, GLFWwindow* shareWith, int windowHeightOffset = 0)
{
    auto monitorMode = glfwGetVideoMode(mon);
    glfwWindowHint(GLFW_RED_BITS, monitorMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, monitorMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, monitorMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, monitorMode->refreshRate);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    auto win = glfwCreateWindow(monitorMode->width, monitorMode->height + windowHeightOffset, name, nullptr, shareWith);
    if (win == nullptr) return nullptr;
    int monX, monY;
    glfwGetMonitorPos(mon, &monX, &monY);
    glfwSetWindowPos(win, monX, monY);

    return win;
}

struct KeyInput
{
    bool left = false, right = false, up = false, down = false;
};

KeyInput inputs;

void KeyboardCallback(GLFWwindow*, int key, int, int action, int)
{
    bool pressed = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key)
    {
    default:
        break;
    case GLFW_KEY_W:
        inputs.up = pressed;
        break;
    case GLFW_KEY_S:
        inputs.down = pressed;
        break;
    case GLFW_KEY_A:
        inputs.left = pressed;
        break;
    case GLFW_KEY_D:
        inputs.right = pressed;
        break;
    }
}

int main()
try
{
	std::ios_base::sync_with_stdio(false);

	glfwSetErrorCallback(glfwExecErrorCallback);
	if (glfwInit() == GLFW_FALSE)
	{
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    int monitorCount;
    auto monitors = glfwGetMonitors(&monitorCount);

    struct WindowData
    {
        GLFWwindow* win;
        GLuint frameBuffer;
        struct Viewport
        {
            GLint x, y;
            GLsizei width, height;
        } viewport;

        glm::vec3 location;
    };

    std::vector<WindowData> windows;
    auto primaryWin = CreateWindowOnMonitor("Bad Window", monitors[0], nullptr, 1);
    if (primaryWin == nullptr)
    {
        return EXIT_FAILURE;
    }
    glfwSetKeyCallback(primaryWin, KeyboardCallback);
    {
        WindowData window;
        window.win = primaryWin;
        windows.push_back(window);
        for (int i = 1; i < monitorCount; ++i)
        {
            auto secondaryWin = CreateWindowOnMonitor("Secondary", monitors[i], primaryWin);
            if (secondaryWin == nullptr) return EXIT_FAILURE;
            window.win = secondaryWin;
            windows.push_back(window);
            glfwSetKeyCallback(secondaryWin, KeyboardCallback);
        }
    }

	glfwMakeContextCurrent(primaryWin);

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED)
	{
		return EXIT_FAILURE;
	}

    GLint curX = 0, curY = 0;

    for (auto& window : windows)
    {
        glfwMakeContextCurrent(window.win);
	    glEnable(GL_DEPTH_TEST);

        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDbgCallback, nullptr);
        //On certain system configurations, this line enables per-frame logging of memory usage on the GPU.
        //This is very spammy on those systems.
        //glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
    }

    //Shared framebuffer for
    int primaryBackWidth, primaryBackHeight;
    glfwGetFramebufferSize(primaryWin, &primaryBackWidth, &primaryBackHeight);
    --primaryBackHeight;

    GLint primaryFrameWidth = primaryBackWidth * 3,
        primaryFrameHeight = primaryBackHeight;

    GLuint primaryTextures[2];
    glGenTextures(2, primaryTextures);
    glBindTexture(GL_TEXTURE_2D, primaryTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, primaryFrameWidth, primaryFrameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, primaryTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, primaryFrameWidth, primaryFrameHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    for (auto& window : windows)
    {
        glfwMakeContextCurrent(window.win);
        glGenFramebuffers(1, &window.frameBuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, window.frameBuffer);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primaryTextures[0], 0);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, primaryTextures[1], 0);
        window.viewport.x = curX;
        window.viewport.y = curY;
        window.viewport.width = primaryBackWidth;
        window.viewport.height = primaryBackHeight;

        curX += primaryBackWidth;
    }

    glfwMakeContextCurrent(primaryWin);
    auto camera = Camera{ Camera::Orthographic{ glm::vec2{ 8.0f, 4.5f } }, -5.0f, 5.0f };
    camera.transform = Transform{ { 0.0f, -1.0f, 0.0f }, glm::quat(),{ 1.0f, 1.0f, 1.0f } };

    //Load model for testing
    Model model;
    std::vector<local_shared_ptr<RenderableHandle>> handles;
    auto renderer = GetRenderManager();
    auto batch = GenerateRenderBatch(renderer);

    {
        Assimp::Importer importer{};
#ifdef _DEBUG
        Assimp::DefaultLogger::create("AssimpLog.txt", Assimp::Logger::VERBOSE, aiDefaultLogStream_STDOUT);
#endif
        auto bunny = importer.ReadFile("./data/models/armadillo.obj", aiProcess_Triangulate | aiProcess_SortByPType
            | aiProcess_GenUVCoords | aiProcess_OptimizeGraph
            | aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals);
        importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 0xffff);
        bunny = importer.ApplyPostProcessing(aiProcess_SplitLargeMeshes | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure);

        if (bunny == nullptr)
        {
            std::string err;
            err += importer.GetErrorString();
            throw std::runtime_error(err);
        }

        Assimp::DefaultLogger::kill();

        const auto& initText = "System Init";

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, sizeof(initText), initText);
        std::vector<Renderable> submeshes;
        submeshes.reserve(bunny->mNumMeshes);
        auto material = GetDefaultMaterial();

        std::vector<std::string> meshNames;
        meshNames.reserve(bunny->mNumMeshes);
        std::transform(bunny->mMeshes, bunny->mMeshes + bunny->mNumMeshes, std::back_inserter(meshNames),
            [](const auto& x)
        {
            return x->mName.C_Str();
        });

        MakeNamesUnique(meshNames.begin(), meshNames.end());

        for (int i = 0; i < int(bunny->mNumMeshes); ++i)
        {
            submeshes.push_back({ RegisterMesh(GetMeshManager(), bunny->mMeshes[i], meshNames[i]),
                nullptr });
        }

        SceneGraph<ModelData> bunnyGraph;
        PopulateGraph(bunnyGraph, bunny->mRootNode);

        model = Model{ submeshes, std::move(bunnyGraph) };

        GlProj::Utilities::TestSceneGraph();

        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

        for (int i = 0; i < int(submeshes.size()); ++i)
        {
            handles.push_back(SubmitRenderable(batch.get(), *(submeshes[i].mesh), submeshes[i].material.get()));
        }
        glFlush();
        auto& hierarchy = model.GetHierarchy();
        for (auto pos = hierarchy.begin(); pos != hierarchy.end(); ++pos)
        {
            if (pos->meshes.empty()) continue;

            auto transform = ApplyHierarchy(*pos.current);
            auto& dat = *pos;
            for (const auto& i : dat.meshes)
            {
                SetTransform(handles[i].get(), transform);
            }
        }
        SetOverrideMaterial(batch.get(), material.get());
        UpdateBatchCamera(batch.get(), camera);
    }

    glPopDebugGroup();
    //

    windows[0].location = { 0.0f, -1.0f, 0.0f };
    windows[1].location = { 8.0f, -1.0f, 0.0f };
    windows[2].location = { -8.0f, -1.0f, 0.0f };

    auto& hierarchy = model.GetHierarchy();
    float angle = 0.0f;
    const float rotationSpeed = 0.6f;

    auto prevTime = glfwGetTime();

    glm::vec3 modelPos{0.0f};
    const float moveSpeed = 5.0f;

    bool run = true;
    while (run)
    {
        glfwMakeContextCurrent(primaryWin);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, windows[0].frameBuffer);
        glViewport(0, 0, primaryFrameWidth, primaryFrameHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Update model transform
        auto newTime = glfwGetTime();
        auto delta = newTime - prevTime;
        prevTime = newTime;
        angle += float(delta) * rotationSpeed;

        if (inputs.up)
        {
            modelPos.y += moveSpeed * float(delta);
        }
        else if (inputs.down)
        {
            modelPos.y -= moveSpeed * float(delta);
        }
        if (inputs.left)
        {
            modelPos.x -= moveSpeed * float(delta);
        }
        else if (inputs.right)
        {
            modelPos.x += moveSpeed * float(delta);
        }

        auto rootTransform = glm::translate(glm::mat4(1), modelPos) * glm::rotate(glm::mat4(1), angle, glm::vec3{ 0.0f, 1.0f, 0.0f });

        for (auto pos = hierarchy.begin(); pos != hierarchy.end(); ++pos)
        {
            if (pos->meshes.empty()) continue;

            auto transform = ApplyHierarchy(*pos.current);
            auto& dat = *pos;
            for (const auto& i : dat.meshes)
            {
                SetTransform(handles[i].get(), rootTransform * transform);
            }
        }
        //

        //Draw to all viewports
        for (auto& window : windows)
        {
            if (glfwWindowShouldClose(window.win))
            {
                run = false;
                break;
            }

            glViewport(window.viewport.x, window.viewport.y, window.viewport.width, window.viewport.height);

            //Update camera and draw per screen region
            camera.transform.position = window.location;
            UpdateBatchCamera(batch.get(), camera);
            Draw(renderer);
            //
            glFlush();
        }

        glFinish();

        for (auto& window : windows)
        {
            glfwMakeContextCurrent(window.win);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(window.viewport.x, window.viewport.y, 
                window.viewport.x + window.viewport.width, window.viewport.y + window.viewport.height,
                0, 0, primaryBackWidth, primaryBackHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glFlush();
        }

        glFinish();

        for (auto& window : windows)
        {
            glfwSwapBuffers(window.win);
        }

        glfwPollEvents();
    }

    for (auto& window : windows)
    {
	    glfwDestroyWindow(window.win);
    }
	glfwTerminate();
}
catch (std::exception&)
{
	LogExceptions();
	throw;
}
