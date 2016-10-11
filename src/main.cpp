#include "gl_core_4_1.h"
#include "GLFW/glfw3.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Camera.hpp"
#include "glm/gtx/transform.hpp"
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

inline glm::mat4 aiToGlm(const aiMatrix4x4& o)
{
	return{o.a1, o.a2, o.a3, o.a4,
		   o.b1, o.b2, o.b3, o.b4,
		   o.c1, o.c2, o.c3, o.c4,
		   o.d1, o.d2, o.d3, o.d4};
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

void PopulateGraph(SceneGraph<ModelData>& graph, 
					GlProj::Utilities::SceneNode<ModelData>* parent, 
					aiNode* node)
{
	parent = graph.emplace(parent, aiToGlm(node->mTransformation),
							std::vector<unsigned int>(node->mMeshes, node->mMeshes + node->mNumMeshes),
							node->mName.C_Str());

	auto children = node->mChildren;
	auto childCount = int(node->mNumChildren);

	for(int i = 0; i < childCount; ++i)
	{
		PopulateGraph(graph, parent, children[i]);
	}
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
	PopulateGraph(bunnyGraph, nullptr, bunny->mRootNode);

	Model bunnyModel{ submeshes, std::move(bunnyGraph) };

	GlProj::Utilities::TestSceneGraph();

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	auto renderer = GetRenderManager();
	auto batch = GenerateRenderBatch(renderer);

	std::vector<local_shared_ptr<RenderableHandle>> handles;
	for (int i = 0; i < int(submeshes.size()); ++i)
	{
		handles.push_back(SubmitRenderable(batch.get(), *(submeshes[i].mesh), submeshes[i].material.get()));
	}
	auto& hierarchy = bunnyModel.GetHierarchy();
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
	auto cam = Camera{ Camera::Orthographic{glm::vec2{16.0f, 9.0f}}, -10.0f, 10.0f};
	UpdateBatchCamera(batch.get(), cam);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
				GL_STENCIL_BUFFER_BIT);

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
