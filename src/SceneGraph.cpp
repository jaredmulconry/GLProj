#include "SceneGraph.hpp"

namespace GlProj
{
	namespace Utilities
	{
		void TestSceneGraph()
		{
			auto t1 = SceneGraph<int>();
			t1.insert(nullptr, 0);
			t1.insert(nullptr, (const int)1);
			auto n3 = t1.emplace(nullptr, 1729);
			auto n1 = t1.find(0);
			auto n2 = t1.find(1);
			t1.emplace(n3, 1);
			t1.emplace(n3, 2);
			t1.emplace(n1, 1);
			t1.emplace(n1, 2);
			auto n4 = t1.emplace(n1, 3);
			t1.insert(n4, 0);
			t1.insert(n4, 1);
			t1.insert(n4, 3);
			t1.insert(n4, 4);
			auto f1 = t1.find(1);
			auto f2 = t1.find(42);
			auto f3 = t1.find_child(n3, 2);
			auto f4 = t1.find_child(n1, 0);
			std::vector<SceneNode<int>*> nodes;
			t1.find_all(0, std::equal_to<>(), nodes);
			nodes.clear();
			t1.find_all_children(n1, 0, std::equal_to<>(), nodes);
			nodes.clear();
			n3 = t1.parent_to(n4, n3);
			t1.remove(t1.find(1729), false);
			t1.remove(n1);
		}
	}
}