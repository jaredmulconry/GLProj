#pragma once
#include <cstddef>
#include <utility>

namespace GlProj
{
	namespace Utilities
	{
		namespace detail
		{
			struct RefBase
			{
				std::size_t ref_count = 1;

				virtual ~RefBase() = default;
			};
			template<typename T>
			struct RefSeparated : public RefBase
			{
				T* data;

				virtual ~RefSeparated()
				{
					delete data;
				}
			};
			template<typename T>
			struct RefJoined : public RefBase
			{
				T data;
				template<typename... Us>
				explicit RefJoined(Us&&... values)
					:data(std::forward<Us>(values)...)
				{}

				virtual ~RefJoined() = default;
			};

			struct MakeFromFunc {};
		}

		template<typename T>
		class LocalSharedPtr
		{
			detail::RefBase* ref;
			T* objRef = nullptr;

			void Increment()
			{
				if (ref == nullptr) return;
				++ref->ref_count;
			}
			void Decrement() noexcept
			{
				if (ref == nullptr) return;
				--ref->ref_count;

				if (ref->ref_count == 0)
				{
					delete ref;
					ref = nullptr;
					objRef = nullptr;
				}
			}
			void AllocateControl(const T& x)
			{
				auto reference = new detail::RefSeparated<T>;
				reference->data = new T(x);
				ref = reference;
				objRef = reference->data;
			}
			void AllocateControl(T&& x)
			{
				auto reference = new detail::RefSeparated<T>;
				reference->data = new T(std::move(x));
				ref = reference;
				objRef = reference->data;
			}
			void AllocateControl(T* x)
			{
				auto reference = new detail::RefSeparated<T>;
				reference->data = x;
				ref = reference;
				objRef = reference->data;
			}
			template<typename... Us>
			void AllocateControlFromArgs(Us&&... values)
			{
				auto reference = new detail::RefJoined<T>(std::forward<Us>(values)...);
				ref = reference;
				objRef = &(reference->data);
			}
		public:
			LocalSharedPtr() noexcept
				: ref(nullptr)
			{}
			LocalSharedPtr(const LocalSharedPtr& x)
				:ref(x.ref)
				,objRef(x.objRef)
			{
				Increment();
			}
			LocalSharedPtr& operator=(const LocalSharedPtr& x)
			{
				if (x.ref != ref)
				{
					Decrement();
					
					ref = x.ref;
					objRef = x.objRef;

					Increment();
				}

				return *this;
			}
			LocalSharedPtr(LocalSharedPtr&& x) noexcept
				:ref(x.ref)
				,objRef(x.objRef)
			{
				x.ref = nullptr;
				x.objRef = nullptr;
			}
			LocalSharedPtr& operator=(LocalSharedPtr&& x) noexcept
			{
				if (x.ref != ref)
				{
					Decrement();

					ref = x.ref;
					objRef = x.objRef;

					x.ref = nullptr;
					x.objRef = nullptr;
				}

				return *this;
			}
			~LocalSharedPtr()
			{
				--ref->ref_count;
				if (ref->ref_count == 0)
				{
					delete ref;
				}
			}

			template<typename... Us>
			explicit LocalSharedPtr(detail::MakeFromFunc, Us&&... values)
			{
				AllocateControlFromArgs(std::forward<Us>(values)...);
			}

			explicit LocalSharedPtr(const T& c)
			{
				AllocateControl(c);
			}
			explicit LocalSharedPtr(T&& c)
			{
				AllocateControl(std::move(c));
			}
			explicit LocalSharedPtr(T* p)
			{
				AllocateControl(p);
			}

			LocalSharedPtr& operator=(const T& c)
			{
				Decrement();
				AllocateControl(c);

				return *this;
			}
			LocalSharedPtr& operator=(T&& c)
			{
				Decrement();
				AllocateControl(std::move(c));

				return *this;
			}

			friend bool operator==(const LocalSharedPtr& x,
				const LocalSharedPtr& y)
			{
				return x.ref == y.ref;
			}
			friend bool operator!=(const LocalSharedPtr& x,
				const LocalSharedPtr& y)
			{
				return !(x == y);
			}

			friend bool operator==(const LocalSharedPtr& x,
				const T* p)
			{
				return x.objRef == p;
			}
			friend bool operator!=(const LocalSharedPtr& x,
				const T* p)
			{
				return !(x == p)
			}
		};

		template<typename T, typename... Us>
		LocalSharedPtr<T> make_localshared(Us&&... values)
		{
			return LocalSharedPtr<T>(detail::MakeFromFunc(), 
									 std::forward<Us>(values)...);
		}
	}
}