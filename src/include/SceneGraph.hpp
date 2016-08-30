#pragma once
#include <algorithm>
#include <iterator>
#include <vector>

struct aiNode;

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

		///T - Composite data type for information stored, per node, in the scene graph
		///Requires T is Semi-regular
		template<typename T>
		class SceneGraph
		{
			using Node = SceneNode<T>;
			using ChildList = std::vector<Node>;
			using ChildListIterator = ChildList::iterator;
			ChildList rootNodes;

			static const constexpr unsigned long ReserveCutoff = 4;

			void UpdateParent(Node* parent, ChildListIterator begin, ChildListIterator end) noexcept
			{
				while (begin != end)
				{
					begin->parent = parent;
					++begin;
				}
			}

			bool ReserveSpaceForChildren(ChildList& dest, ChildList::size_type count)
			{
				if (count <= 0) return;
				auto newSize = dest.size() + count;
				auto previousCapacity = dest.capacity();

				bool previousNeedUpdate = newSize > previousCapacity;
				if (previousNeedUpdate)
				{
					if (newSize - previousCapacity >= ReserveCutoff)
					{
						dest.reserve(newSize);
					}
				}

				return previousNeedUpdate;
			}

			void OrphanChildren(Node& src)
			{
				auto previousSize = rootNodes.size();
				ReserveSpaceForChildren(rootNodes, src.children->size());
				std::move(src.children->begin(), src.children->end(), std::make_move_iterator(std::back_inserter(rootNodes)));

				UpdateParent(nullptr, (rootNodes.begin() + previousSize), rootNodes.end());
				src.children->clear();
			}

			void MergeChildren(Node* dest, Node& src)
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

			void DestroySubTree(Node& n)
			{
				if (n.children->size() != 0)
				{
					auto current = &(*n.children)[0];
					while (true)
					{
						auto distanceToEnd = current - &(*current->parent->children)[0];
						if (distanceToEnd >= current->parent->children->size())
						{
							current = current->parent;
							delete current->children;
							if (current == &n) break;
							++current;
							continue;
						}
						if (current->children->size() > 0)
						{
							current = 
						}
						else
						{

						}
					}
				}
				delete n.children;
			}
		public:
			SceneGraph() noexcept = default;
			~SceneGraph()
			{
				if (rootNodes.size() == 0) return;

				for (auto& n : rootNodes)
				{
					DestroySubTree(n);
				}
			}
		};
	}
}
