#pragma once
#include <algorithm>
#include <iterator>
#include <vector>

namespace GlProj
{
	namespace Utilities
	{
		///Requires T is Semi-regular
		template<typename T>
		struct SceneNode
		{
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
		private:
			using ChildList = std::vector<node_type>;
			using ChildListIterator = typename ChildList::iterator;
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
				if (count <= 0) return;
				auto newSize = dest.size() + count;
				auto previousCapacity = dest.capacity();

				bool previousNeedUpdate = newSize > previousCapacity;
				if (previousNeedUpdate)
				{
					dest.reserve(newSize);
				}

				return previousNeedUpdate;
			}

			void OrphanChildren(node_type& src)
			{
				auto previousSize = rootNodes.size();
				ReserveSpaceForChildren(rootNodes, src.children->size());
				std::move(src.children->begin(), src.children->end(), std::make_move_iterator(std::back_inserter(rootNodes)));

				UpdateParent(nullptr, (rootNodes.begin() + previousSize), rootNodes.end());
				src.children->clear();
			}

			void MergeChildren(node_type* dest, node_type& src)
			{
				if (dest == null)
				{
					OrphanChildren(src);
					return;
				}
				auto& sourceChildren = *src.children;
				auto& destChildren = *dest->children;

				auto previousSize = destChildren.size();
				bool previousNeedUpdate = ReserveSpaceForChildren(destChildren, sourceChildren.size());
				std::move(sourceChildren.begin(), sourceChildren.end(), std::make_move_iterator(std::back_inserter(destChildren)));

				auto parentUpdateBegin = previousNeedUpdate ? destChildren.begin() : (destChildren.begin() + previousSize);
				UpdateParent(dest, parentUpdateBegin, destChildren.end());
				sourceChildren.clear();
			}

			void DestroySubTree(node_type& n)
			{
				if (n.children->size() != 0)
				{
					//Point at each child/sub-child. Delete from the leaves.
					//Increment until past-the-end is reached. Move up to
					//the parent, delete its children, advance to the neighbour.
					//Continue until we reach the root of the tree.
					auto current = &(*n.children)[0];
					while (true)
					{
						auto distanceToEnd = current - &(*current->parent->children)[0];
						if (distanceToEnd >= current->parent->children->size())
						{
							current = current->parent;
							if (current == &n) break;
						KillChildrenAndAdvance:
							KillChildren(*current);
							++current;
							continue;
						}
						if (current->children->size() > 0)
						{
							current = &(*current->children[0]);
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
		public:
			

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
				auto finder = [&x](const auto& a)
				{
					return a.data == x;
				};
				auto found = std::find_if(rootNodes.begin(), rootNodes.end(), finder);
				if (found != rootNodes.end()) return &*found;

				for (auto& c : allChildren)
				{
					auto foundInChildren = std::find_if(c->begin(), c->end(), finder);
					if (foundInChildren != c->end()) return &*foundInChildren;
				}

				return nullptr;
			}

			void insert(node_type* parent, const T& data)
			{
				ChildList& children = (parent == nullptr) ? rootNodes : *parent->children;

				auto needUpdate = ReserveSpaceForChildren(children, 1);
				children.push_back({data, parent, CreateChildList()});

				if (needUpdate && parent != nullptr)
				{
					UpdateParent(parent, children.begin(), children.end() - 1);
				}
			}

			node_type* remove(node_type* n, bool deleteSubtree = true)
			{
				auto parent = n->parent;
				if (deleteSubtree)
				{
					DestroySubTree(n);
					DeleteNode(n);
				}
				else
				{
					MergeChildren(n->parent, n);
					DeleteNode(n);
				}
				return parent;
			}
		};
	}
}
