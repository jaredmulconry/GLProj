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

static const constexpr bool dumpCapabilities = true;

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
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
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

	if (dumpCapabilities)
	{
		int contextMajorVersion = glfwGetWindowAttrib(win, GLFW_CONTEXT_VERSION_MAJOR);
		int contextMinorVersion = glfwGetWindowAttrib(win, GLFW_CONTEXT_VERSION_MINOR);
		int maxVertAttribs = 0, maxVertAttribBinds = 0;
		int maxVertUniformBlocks = 0, maxVertUniformComps = 0, maxVertUniformVecs = 0;


		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertAttribs);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &maxVertAttribBinds);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxVertUniformBlocks);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertUniformComps);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformVecs);

		std::cout << "Major version is " << contextMajorVersion << '\n';
		std::cout << "Minor version is " << contextMinorVersion << '\n';
		std::cout << "GL_MAX_VERTEX_ATTRIBS " << maxVertAttribs << '\n';
		std::cout << "GL_MAX_VERTEX_ATTRIB_BINDINGS " << maxVertAttribBinds << '\n';
		std::cout << "GL_MAX_VERTEX_UNIFORM_BLOCKS " << maxVertUniformBlocks << '\n';
		std::cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS " << maxVertUniformComps << '\n';
		std::cout << "GL_MAX_VERTEX_UNIFORM_VECTORS " << maxVertUniformVecs << '\n';
	}

	glEnable(GL_DEPTH_TEST);

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GLDbgCallback, nullptr);
	//On certain system configurations, this line enables per-frame logging of memory usage on the GPU.
	//This is very spammy on those systems.
	//glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

	PrepareAndRunGame(win);

	glfwDestroyWindow(win);
	glfwTerminate();
}
catch (std::exception&)
{
	LogExceptions();
	throw;
}
