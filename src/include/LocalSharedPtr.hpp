#pragma once
#include <cstddef>
#include <functional>
#include <type_traits>
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

			void InternalReset()
			{
				InternalReset(nullptr, nullptr);
			}
			void InternalReset(T* d, detail::RefBase* rc)
			{
				Decrement();

				ref = rc;
				objRef = d;
			}

			void AllocateControl(const T& x)
			{
				auto reference = new detail::RefSeparated<T>;
				reference->data = new T(x);
				InternalReset(reference->data, reference);
			}
			void AllocateControl(T&& x)
			{
				auto reference = new detail::RefSeparated<T>;
				reference->data = new T(std::move(x));
				InternalReset(reference->data, reference);
			}
			void AllocateControl(T* x)
			{
				if (x == nullptr)
				{
					InternalReset();
				}
				else
				{
					auto reference = new detail::RefSeparated<T>;
					reference->data = x;
					InternalReset(reference->data, reference);
				}
			}
			template<typename... Us>
			void AllocateControlFromArgs(Us&&... values)
			{
				auto reference = new detail::RefJoined<T>(std::forward<Us>(values)...);
				InternalReset(&(reference->data), reference);
			}
		public:
			LocalSharedPtr() noexcept
				: ref(nullptr)
			{}
			LocalSharedPtr(const LocalSharedPtr& x)
				:ref(x.ref)
				, objRef(x.objRef)
			{
				Increment();
			}
			LocalSharedPtr& operator=(const LocalSharedPtr& x)
			{
				if (x.ref != ref)
				{
					InternalReset(x.objRef, x.ref);

					Increment();
				}

				return *this;
			}
			LocalSharedPtr(LocalSharedPtr&& x) noexcept
				:ref(x.ref)
				, objRef(x.objRef)
			{
				x.ref = nullptr;
				x.objRef = nullptr;
			}
			LocalSharedPtr& operator=(LocalSharedPtr&& x) noexcept
			{
				if (x.ref != ref)
				{
					InternalReset(x.objRef, x.ref);

					x.ref = nullptr;
					x.objRef = nullptr;
				}

				return *this;
			}
			~LocalSharedPtr()
			{
				Decrement();
			}

			template<typename... Us>
			explicit LocalSharedPtr(detail::MakeFromFunc, Us&&... values)
			{
				AllocateControlFromArgs(std::forward<Us>(values)...);
			}

			explicit LocalSharedPtr(T* p)
			{
				AllocateControl(p);
			}

			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>>>
				LocalSharedPtr& operator=(const LocalSharedPtr<U>& x)
			{
				if (x.ref != ref)
				{
					InternalReset(x.objRef, x.ref);

					Increment();
				}
			}
			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>>>
			LocalSharedPtr& operator=(LocalSharedPtr<U>&& x)
			{
				if (x.ref != ref)
				{
					InternalReset(x.objRef, x.ref);

					x.objRef = nullptr;
					x.ref = nullptr;
				}
			}

			T* get() const noexcept
			{
				return objRef;
			}

			T& operator*() const
			{
				return *objRef;
			}
			T* operator->() const
			{
				return objRef;
			}

			long use_count() const
			{
				return ref == nullptr ? 0l : long(ref->ref_count);
			}
			bool unique() const
			{
				return use_count() == 1;
			}
			explicit operator bool() const
			{
				return get() != nullptr;
			}

			void reset()
			{
				InternalReset();
			}
			void reset(T* ptr)
			{
				AllocateControl(ptr);
			}

			template<typename U>
			bool owner_before(const LocalSharedPtr<U>& x)
			{
				return ref < x.ref;
			}

			template<typename U1, typename U2>
			friend bool operator==(const LocalSharedPtr<U1>& x,
				const LocalSharedPtr<U2>& y) noexcept
			{
				return x.get() == y.get();
			}
			template<typename U1>
			friend bool operator==(const LocalSharedPtr<U1>& x, 
								   std::nullptr_t npt) noexcept
			{
				return x.get() == npt;
			}
			template<typename U1>
			friend bool operator==(std::nullptr_t npt,
								   const LocalSharedPtr<U1>& x) noexcept
			{
				return x == npt;
			}

			template<typename U1, typename U2>
			friend bool operator!=(const LocalSharedPtr<U1>& x,
				const LocalSharedPtr<U2>& y) noexcept
			{
				return !(x == y);
			}
			template<typename U1>
			friend bool operator!=(const LocalSharedPtr<U1>& x,
				std::nullptr_t npt) noexcept
			{
				return !(x == npt);
			}
			template<typename U1>
			friend bool operator!=(std::nullptr_t npt,
				const LocalSharedPtr<U1>& x) noexcept
			{
				return x != npt;
			}

			template<typename U1, typename U2>
			friend bool operator<(const LocalSharedPtr<U1>& x,
				const LocalSharedPtr<U2>& y) noexcept
			{
				return std::less<
					std::common_type_t<decltype(x.get()),
									   decltype(y.get())>>()(x.get(), y.get());
			}
			template<typename U1>
			friend bool operator<(const LocalSharedPtr<U1>& x,
				std::nullptr_t npt) noexcept
			{
				return std::less<U1*>()(x.get(), (U1*)npt);
			}
			template<typename U1>
			friend bool operator<(std::nullptr_t npt,
				const LocalSharedPtr<U1>& y) noexcept
			{
				return std::less<U1*>()((U1*)npt, y.get());
			}
			
			template<typename U1, typename U2>
			friend bool operator<=(const LocalSharedPtr<U1>& x,
				const LocalSharedPtr<U2>& y) noexcept
			{
				return !(y < x);
			}
			template<typename U1>
			friend bool operator<=(const LocalSharedPtr<U1>& x,
				std::nullptr_t npt) noexcept
			{
				return !(npt < x.get());
			}
			template<typename U1>
			friend bool operator<=(std::nullptr_t npt,
				const LocalSharedPtr<U1>& y) noexcept
			{
				return !(y.get() < npt);
			}

			template<typename U1, typename U2>
			friend bool operator>(const LocalSharedPtr<U1>& x,
				const LocalSharedPtr<U2>& y) noexcept
			{
				return y < x;
			}
			template<typename U1>
			friend bool operator>(const LocalSharedPtr<U1>& x,
				std::nullptr_t npt) noexcept
			{
				return npt < x;
			}
			template<typename U1>
			friend bool operator>(std::nullptr_t npt,
				const LocalSharedPtr<U1>& y) noexcept
			{
				return y < npt;
			}

			template<typename U1, typename U2>
			friend bool operator>=(const LocalSharedPtr<U1>& x,
				const LocalSharedPtr<U2>& y) noexcept
			{
				return !(x < y);
			}
			template<typename U1>
			friend bool operator>=(const LocalSharedPtr<U1>& x,
				std::nullptr_t npt) noexcept
			{
				return !(x < npt);
			}
			template<typename U1>
			friend bool operator>=(std::nullptr_t npt,
				const LocalSharedPtr<U1>& y) noexcept
			{
				return !(npt < y);
			}

			friend void swap(LocalSharedPtr& x, LocalSharedPtr& y) noexcept
			{
				using std:swap;

				swap(x.ref, y.ref);
				swap(x.objRef, y.objRef);
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