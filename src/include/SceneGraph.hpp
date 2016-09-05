#pragma once
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace GlProj
{
	namespace Utilities
	{
		namespace detail
		{
			template<typename...>
			using void_t = void;

			template<typename C, typename, typename...>
			struct is_callable_helper : std::false_type
			{};

			template<typename C, typename... ArgTypes>
			struct is_callable_helper<C, void_t<decltype(std::declval<C>()(std::declval<ArgTypes>()...))>, ArgTypes...>
				: std::true_type
			{};

			template<typename C, typename... Args>
			using is_callable = is_callable_helper<C, void, Args...>;
		}

		///Requires T is Semi-regular
		template<typename T>
		struct SceneNode
		{
			SceneNode() noexcept = default;
			template<typename... Us>
			SceneNode(SceneNode* p, std::vector<SceneNode>* c, Us&&... values)
				:data(std::forward<Us>(values)...)
				, parent(p)
				, children(c)
			{}

			T data;
			SceneNode* parent;
			std::vector<SceneNode>* children;
		};

		template<typename T>
		bool operator==(const SceneNode<T>& x, const SceneNode<T>& y)
		{
			return x.children == y.children;
		}
		template<typename T>
		bool operator!=(const SceneNode<T>& x, const SceneNode<T>& y)
		{
			return !(x == y);
		}

		///T - Composite data type for information stored, per node, in the scene graph
		///Requires T is Semi-regular
		template<typename T>
		class SceneGraph
		{
		public:
			using node_type = SceneNode<T>;
			using ChildList = std::vector<node_type>;
			using ChildListIterator = typename ChildList::iterator;

			SceneGraph() noexcept = default;
			~SceneGraph()
			{
				for (auto& n : allChildren)
				{
					delete n;
				}
			}

			template<typename U>
			node_type* find(const U& x) const
			{
				return find(x, std::equal_to<>());
			}
			template<typename U, typename C>
			node_type* find(const U& x, C c) const
			{
				auto finder = [&x, &c](const auto& a)
				{
					return c(a.data, x);
				};
				auto found = std::find_if(rootNodes.begin(), rootNodes.end(), finder);
				if (found != rootNodes.end()) return const_cast<node_type*>(&*found);

				for (auto& c : allChildren)
				{
					auto foundInChildren = std::find_if(c->begin(), c->end(), finder);
					if (foundInChildren != c->end()) return const_cast<node_type*>(&*foundInChildren);
				}

				return nullptr;
			}

			template<typename U>
			node_type* find_child(node_type* p, const U& x) const
			{
				return find_child(p, x, std::equal_to<>());
			}
			template<typename U, typename C>
			node_type* find_child(node_type* p, const U& x, C c) const
			{
				if (p == nullptr)
				{
					return find(x, c);
				}

				if (p->children->empty())
				{
					return nullptr;
				}

				auto current = &(*p->children)[0];
				auto currentParent = p;
				while (true)
				{
					auto distanceToEnd = size_type(current - &(*currentParent->children)[0]);
					if (distanceToEnd >= currentParent->children->size())
					{
						current = currentParent;
						currentParent = current->parent;
						if (current == p) break;
					MoveToNextSibling:
						++current;
						continue;
					}
					if (c(current->data, x))
					{
						return current;
					}
					if (!current->children->empty())
					{
						current = &((*current->children)[0]);
						currentParent = current->parent;
					}
					else
					{
						goto MoveToNextSibling;
					}
				}
				return nullptr;
			}


			template<typename U>
			std::vector<node_type*> find_all(const U& x) const
			{
				std::vector<node_type*> o;
				find_all(x, o);
				return o;
			}
			template<typename U>
			void find_all(const U& x, std::vector<node_type*>& out) const
			{
				find_all(x, std::equal_to<>(), out);
			}

			template<typename U, typename C,
				typename = std::enable_if_t<detail::is_callable<C, T&, const U&>::value>>
				std::vector<node_type*> find_all(const U& x, C c) const
			{
				std::vector<node_type*> o;
				find_all(x, c, o);
				return o;
			}
			template<typename U, typename C>
			void find_all(const U& x, C c, std::vector<node_type*>& out) const
			{
				auto& result = out;
				auto insertIterator = std::back_inserter(result);
				auto filler = [&x, &c, &insertIterator](auto& a)
				{
					if (c(a.data, x))
					{
						*insertIterator = const_cast<node_type*>(&a);
						++insertIterator;
					}
				};

				std::for_each(rootNodes.begin(), rootNodes.end(), filler);

				for (auto& c : allChildren)
				{
					std::for_each(c->begin(), c->end(), filler);
				}
			}

			template<typename U>
			std::vector<node_type*> find_all_children(node_type* p, const U& x) const
			{
				std::vector<node_type*> o;
				find_all_children(p, x, o);
				return o;
			}
			template<typename U>
			void find_all_children(node_type* p, const U& x, std::vector<node_type*>& out) const
			{
				find_all_children(p, x, std::equal_to<>(), out);
			}

			template<typename U, typename C,
				typename = std::enable_if_t<detail::is_callable<C, T&, const U&>::value>>
				std::vector<node_type*> find_all_children(node_type* p, const U& x, C c) const
			{
				std::vector<node_type*> o;
				find_all_children(p, x, c, o);
				return o;
			}
			template<typename U, typename C>
			void find_all_children(node_type* p, const U& x, C c, std::vector<node_type*>& out) const
			{
				if (p == nullptr)
				{
					find_all(x, c, out);
				}

				if (p->children->empty())
				{
					return;
				}

				auto current = &(*p->children)[0];
				auto currentParent = p;
				while (true)
				{
					auto distanceToEnd = size_type(current - &(*currentParent->children)[0]);
					if (distanceToEnd >= currentParent->children->size())
					{
						current = currentParent;
						currentParent = current->parent;
						if (current == p) break;
					MoveToNextSibling:
						++current;
						continue;
					}
					if (c(current->data, x))
					{
						out.push_back(current);
					}
					if (!current->children->empty())
					{
						current = &((*current->children)[0]);
						currentParent = current->parent;
					}
					else
					{
						goto MoveToNextSibling;
					}
				}
			}

			node_type* insert(node_type* parent, const T& data)
			{
				ChildList& children = (parent == nullptr) ? rootNodes : *parent->children;

				auto needUpdate = ReserveSpaceForChildren(children, 1);
				children.emplace_back(parent, CreateChildList(), data);

				if (needUpdate && parent != nullptr)
				{
					UpdateParent(parent, children.begin(), children.end() - 1);
				}

				return &children.back();
			}
			node_type* insert(node_type* parent, T&& data)
			{
				ChildList& children = (parent == nullptr) ? rootNodes : *parent->children;

				auto needUpdate = ReserveSpaceForChildren(children, 1);
				children.emplace_back(parent, CreateChildList(), std::move(data));

				if (needUpdate && parent != nullptr)
				{
					UpdateParent(parent, children.begin(), children.end() - 1);
				}

				return &children.back();
			}
			template<typename... Us>
			node_type* emplace(node_type* parent, Us&&... values)
			{
				ChildList& children = (parent == nullptr) ? rootNodes : *parent->children;

				auto needUpdate = ReserveSpaceForChildren(children, 1);
				children.emplace_back(parent, CreateChildList(), std::forward<Us>(values)...);

				if (needUpdate && parent != nullptr)
				{
					UpdateParent(parent, children.begin(), children.end() - 1);
				}

				return &children.back();
			}

			node_type* remove(node_type* n, bool deleteSubtree = true)
			{
				auto parent = n->parent;
				if (deleteSubtree)
				{
					DestroySubTree(*n);
					DeleteNode(*n);
				}
				else
				{
					n = MergeChildren(n->parent, n);
					DeleteNode(*n);
				}
				return parent;
			}

			///You will not be protected from attaching a node to one of its
			///children. Doing so will result in undefined behaviour.
			node_type* parent_to(node_type* newParent, node_type* n)
			{
				if (n->parent == newParent) return n;

				auto& dest = (newParent == nullptr) ? rootNodes : *newParent->children;
				auto& src = (n->parent == nullptr) ? rootNodes : *n->parent->children;

				auto needsUpdate = ReserveSpaceForChildren(dest, 1);
				auto removePoint = std::find(src.begin(), src.end(), *n);
				dest.push_back(std::move(*n));
				n = &dest.back();
				auto srcUpdatePt = src.erase(removePoint);
				while (srcUpdatePt != src.end())
				{
					UpdateParent(&*srcUpdatePt, srcUpdatePt->children->begin(), srcUpdatePt->children->end());
					++srcUpdatePt;
				}

				auto destUpdatePt = needsUpdate ? dest.begin() : (dest.begin() + dest.size()-1);
				while (destUpdatePt != dest.end())
				{
					UpdateParent(&*destUpdatePt, destUpdatePt->children->begin(), destUpdatePt->children->end());
					++destUpdatePt;
				}
				UpdateParent(&dest.back(), dest.back().children->begin(), dest.back().children->end());
				n->parent = newParent;
				return n;
			}
		private:
			using size_type = typename ChildList::size_type;
			ChildList rootNodes;
			std::vector<ChildList*> allChildren;

			void UpdateParent(node_type* parent, ChildListIterator begin, ChildListIterator end) noexcept
			{
				while (begin != end)
				{
					begin->parent = parent;
					++begin;
				}
			}

			bool ReserveSpaceForChildren(ChildList& dest, size_type count)
			{
				if (count <= 0) return false;
				auto newSize = dest.size() + count;
				auto previousCapacity = dest.capacity();

				bool previousNeedUpdate = newSize > previousCapacity;
				if (previousNeedUpdate)
				{
					dest.reserve(newSize);
				}

				return previousNeedUpdate;
			}

			node_type* ConvertRefToNode(const std::pair<ChildList*, int>& ref)
			{
				return &(*ref.first)[ref.second];
			}
			std::pair<ChildList*, int> GetSemistableRef(node_type& n)
			{
				auto& childList = (n.parent == nullptr) ? rootNodes : *n.parent->children;
				auto pos = std::find(childList.begin(), childList.end(), n);
				auto offset = int(pos - childList.begin());
				return std::make_pair(&childList, offset);
			}

			node_type* MergeChildren(node_type* dest, node_type* src)
			{
				auto& destChildren = (dest == nullptr) ? rootNodes : *dest->children;
				auto& srcChildren = *src->children;

				auto stableSrc = GetSemistableRef(*src);

				bool destContainsSrc = std::find(destChildren.begin(), destChildren.end(), *src) != destChildren.end();

				auto previousSize = destChildren.size();
				bool previousNeedUpdate = ReserveSpaceForChildren(destChildren, srcChildren.size());
				std::move(srcChildren.begin(), srcChildren.end(), std::back_inserter(destChildren));

				auto parentUpdateBegin = previousNeedUpdate ? destChildren.begin() : (destChildren.begin() + previousSize);
				UpdateParent(dest, parentUpdateBegin, destChildren.end());
				srcChildren.clear();

				if (destContainsSrc)
				{
					src = ConvertRefToNode(stableSrc);
				}

				return src;
			}

			void DestroySubTree(node_type& n)
			{
				if (!n.children->empty())
				{
					//Point at each child/sub-child. Delete from the leaves.
					//Increment until past-the-end is reached. Move up to
					//the parent, delete its children, advance to the neighbour.
					//Continue until we reach the root of the tree.
					auto current = &(*n.children)[0];
					auto currentParent = &n;
					while (true)
					{
						auto distanceToEnd = size_type(current - &(*currentParent->children)[0]);
						if (distanceToEnd >= currentParent->children->size())
						{
							current = currentParent;
							currentParent = current->parent;
							if (current == &n) break;
						KillChildrenAndAdvance:
							KillChildren(*current);
							++current;
							continue;
						}
						if (!current->children->empty())
						{
							current = &((*current->children)[0]);
							currentParent = current->parent;
						}
						else
						{
							goto KillChildrenAndAdvance;
						}
					}
				}
			}

			void KillChildren(node_type& n)
			{
				allChildren.erase(std::find(allChildren.begin(), allChildren.end(), n.children));
				delete n.children;
			}

			void DetachNodeFromParent(node_type& n)
			{
				ChildList& children = (n.parent == nullptr) ? rootNodes : *n.parent->children;
				auto childPos = children.erase(std::find(children.begin(), children.end(), n));

				while (childPos != children.end())
				{
					UpdateParent(&*childPos, childPos->children->begin(), childPos->children->end());
					++childPos;
				}
			}

			void DeleteNode(node_type& n)
			{
				KillChildren(n);
				DetachNodeFromParent(n);
			}

			ChildList* CreateChildList()
			{
				allChildren.push_back(new ChildList);
				return allChildren.back();
			}
		};

		void TestSceneGraph();
	}
}
