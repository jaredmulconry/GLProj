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
		template<typename T>
		class SceneGraph;
		template<typename T>
		struct SceneGraphIterator;
		template<typename T>
		struct SceneGraphConstIterator;

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

		///Depth-first iterator over a scene graph
		template<typename T>
		struct SceneGraphIterator :
			public std::iterator<std::forward_iterator_tag,
			T>
		{
			friend SceneGraph<T>;
			friend SceneGraphConstIterator<T>;
			using base_t = std::iterator<std::forward_iterator_tag,
				T>;
			using reference = typename base_t::reference;
			using pointer = typename base_t::reference;

			SceneNode<T>* current;
			SceneGraph<T>* owner;

			SceneGraphIterator() noexcept = default;
			SceneGraphIterator(SceneNode<T>* current, SceneGraph<T>* owner) noexcept
				:current(current)
				, owner(owner)
			{}
			explicit SceneGraphIterator(const SceneGraphIterator<T>& x)
				:current(x.current)
				, owner(x.owner)
			{}

			SceneGraphIterator& operator=(const SceneGraphIterator<T>& x)
			{
				current = x.current;
				owner = x.owner;

				return *this;
			}


			reference operator*()
			{
				return current->data;
			}
			reference operator*() const
			{
				return current->data;
			}

			pointer operator->()
			{
				return &current->data;
			}
			pointer operator->()const
			{
				return &current->data;
			}

			SceneGraphIterator& operator++();
			SceneGraphIterator operator++(int)
			{
				auto cpy = *this;
				++(*this);
				return cpy;
			}

			friend bool operator==(const SceneGraphIterator& x, const SceneGraphIterator& y) noexcept
			{
				return x.current == y.current && x.owner == y.owner;
			}
			friend bool operator!=(const SceneGraphIterator& x, const SceneGraphIterator& y) noexcept
			{
				return !(x == y);
			}
		};
		///Depth-first iterator over a scene graph
		template<typename T>
		struct SceneGraphConstIterator :
			public std::iterator<std::forward_iterator_tag,
			const T>
		{
			friend SceneGraph<T>;
			friend SceneGraphIterator<T>;
			using base_t = std::iterator<std::forward_iterator_tag,
				T>;
			using reference = typename base_t::reference;
			using pointer = typename base_t::pointer;
			SceneNode<T>* current;
			SceneGraph<T>* owner;

			SceneGraphConstIterator() noexcept = default;
			SceneGraphConstIterator(SceneNode<T>* current, SceneGraph<T>* owner) noexcept
				:current(current)
				, owner(owner)
			{}
			explicit SceneGraphConstIterator(const SceneGraphIterator<T>& x)
				:current(x.current)
				, owner(x.owner)
			{}

			SceneGraphConstIterator& operator=(const SceneGraphIterator<T>& x)
			{
				current = x.current;
				owner = x.owner;

				return *this;
			}

			reference operator*()
			{
				return current->data;
			}
			reference operator*() const
			{
				return current->data;
			}

			pointer operator->()
			{
				return &current->data;
			}
			pointer operator->()const
			{
				return &current->data;
			}

			SceneGraphConstIterator& operator++();
			SceneGraphConstIterator operator++(int)
			{
				auto cpy = *this;
				++(*this);
				return cpy;
			}

			friend bool operator==(const SceneGraphConstIterator& x, const SceneGraphConstIterator& y) noexcept
			{
				return x.current == y.current && x.owner == y.owner;
			}
			friend bool operator!=(const SceneGraphConstIterator& x, const SceneGraphConstIterator& y) noexcept
			{
				return !(x == y);
			}
		};

		///T - Composite data type for information stored, per node, in the scene graph
		///Requires T is Semi-regular
		template<typename T>
		class SceneGraph
		{
		public:
			friend SceneGraphIterator<T>;
			friend SceneGraphConstIterator<T>;

			using node_type = SceneNode<T>;
			using ChildList = std::vector<node_type>;
			using ChildListIterator = typename ChildList::iterator;
			using GraphIterator = SceneGraphIterator<T>;
			using GraphConstIterator = SceneGraphConstIterator<T>;

			SceneGraph() noexcept = default;
			~SceneGraph()
			{
				for (auto& n : allChildren)
				{
					delete n;
				}
			}
			SceneGraph(SceneGraph&& x) noexcept;
			SceneGraph& operator=(SceneGraph&& x) noexcept;

			GraphIterator begin()
			{
				return{ rootNodes.data(),
						this };
			}
			GraphConstIterator begin() const
			{
				return{ const_cast<node_type*>(rootNodes.data()),
						const_cast<SceneGraph<T>*const>(this) };
			}
			GraphConstIterator cbegin() const
			{
				return begin();
			}

			GraphIterator end()
			{
				return{ rootNodes.data() + rootNodes.size(),
						this };
			}
			GraphConstIterator end() const
			{
				return{ const_cast<node_type*>(rootNodes.data() + rootNodes.size()),
						const_cast<SceneGraph<T>*const>(this) };
			}
			GraphConstIterator cend() const
			{
				return end();
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
			void find_all_cached(const U& x, std::vector<node_type*>& out) const
			{
				find_all_cached(x, std::equal_to<>(), out);
			}

			template<typename U, typename C>
			std::vector<node_type*> find_all(const U& x, C c) const
			{
				std::vector<node_type*> o;
				find_all_cached(x, c, o);
				return o;
			}
			template<typename U, typename C>
			void find_all_cached(const U& x, C c, std::vector<node_type*>& out) const
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
			void find_all_children_cached(node_type* p, const U& x, std::vector<node_type*>& out) const
			{
				find_all_children(p, x, std::equal_to<>(), out);
			}

			template<typename U, typename C>
			std::vector<node_type*> find_all_children(node_type* p, const U& x, C c) const
			{
				std::vector<node_type*> o;
				find_all_children_cached(p, x, c, o);
				return o;
			}
			template<typename U, typename C>
			void find_all_children_cached(node_type* p, const U& x, C c, std::vector<node_type*>& out) const
			{
				if (p == nullptr)
				{
					find_all_cached(x, c, out);
					return;
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

				if (needUpdate)
				{
					UpdateParent(children.begin(), children.end() - 1);
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
					UpdateParent(children.begin(), children.end() - 1);
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
					UpdateParent(children.begin(), children.end() - 1);
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
				UpdateParent(srcUpdatePt, src.end());

				auto destUpdatePt = needsUpdate ? dest.begin() : (dest.begin() + dest.size() - 1);
				UpdateParent(destUpdatePt, dest.end());

				//UpdateParent(&dest.back(), dest.back().children->begin(), dest.back().children->end());
				UpdateParent(dest.end() - 1, dest.end());
				n->parent = newParent;
				return n;
			}

			bool verify_integrity() const;
		private:
			using size_type = typename ChildList::size_type;
			ChildList rootNodes;
			std::vector<ChildList*> allChildren;

			void UpdateParent(ChildListIterator begin, ChildListIterator end) noexcept
			{
				while (begin != end)
				{
					if (!begin->children->empty())
					{
						auto parent = &(*begin);
						UpdateParent(parent, begin->children->begin(), begin->children->end());
					}
					++begin;
				}
			}

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
				UpdateParent(parentUpdateBegin, destChildren.end());
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

				UpdateParent(childPos, children.end());
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

		template<typename T>
		inline SceneGraphConstIterator<T> & SceneGraphConstIterator<T>::operator++()
		{
			if (!current->children->empty())
			{
				current = current->children->data();
			}
			else
			{
				auto* selfList = (current->parent == nullptr) ? &owner->rootNodes : current->parent->children;

				auto prev = current++;
				while (current == (selfList->data() + selfList->size()))
				{
					if (prev->parent == nullptr) return *this;

					selfList = (prev->parent->parent == nullptr) ? &owner->rootNodes : prev->parent->parent->children;
					auto newPrev = prev->parent;
					current = newPrev + 1;
					prev = newPrev;
				}
			}

			return *this;
		}
		template<typename T>
		inline SceneGraphIterator<T> & SceneGraphIterator<T>::operator++()
		{
			if (!current->children->empty())
			{
				current = current->children->data();
			}
			else
			{
				auto* selfList = (current->parent == nullptr) ? &owner->rootNodes : current->parent->children;

				++current;
				while (current == (selfList->data() + selfList->size()))
				{
					if (current->parent == nullptr) return *this;

					selfList = (current->parent->parent == nullptr) ? &owner->rootNodes : current->parent->parent->children;
					current = current->parent + 1;
				}
			}

			return *this;
		}
		template<typename T>
		inline SceneGraph<T>::SceneGraph(SceneGraph<T>&& x) noexcept
			: rootNodes(std::move(x.rootNodes))
			, allChildren(std::move(x.allChildren))
		{
			for (auto& node : rootNodes)
			{
				auto parent = &node;

				UpdateParent(parent, node.children->begin(), node.children->end());
			}
		}
		template<typename T>
		inline SceneGraph<T>& SceneGraph<T>::operator=(SceneGraph<T>&& x) noexcept
		{
			if (this != &x)
			{
				for (auto& n : allChildren)
				{
					delete n;
				}

				rootNodes = std::move(x.rootNodes);
				allChildren = std::move(x.allChildren);

				for (auto& node : rootNodes)
				{
					auto parent = &node;

					UpdateParent(parent, node.children->begin(), node.children->end());
				}
			}
			return *this;
		}
		template<typename T>
		inline bool SceneGraph<T>::verify_integrity() const
		{
			for (auto& p : allChildren)//vector<SceneNode>*
			{
				for (auto& c : *p)//SceneNode
				{
					for (auto& cc : *c.children)//SceneNode
					{
						assert(cc.parent == &c);
					}
				}
			}
			return true;
		}
	}
}
