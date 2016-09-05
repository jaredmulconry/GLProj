#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <new>
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
				std::int_fast32_t ref_count = 1;
				std::int_fast32_t weak_count = 0;

				virtual void Destroy() = 0;
				virtual ~RefBase() = default;
			};
			template<typename T>
			struct RefSeparated final : public RefBase
			{
				T* data;

				// Inherited via RefBase
				virtual void Destroy() override
				{
					delete data;
				}
				virtual ~RefSeparated() = default;
			};
			template<typename T>
			struct RefJoined final : public RefBase
			{
				alignas(T) unsigned char data[sizeof(T)];
				//T data;
				template<typename... Us>
				explicit RefJoined(Us&&... values)
				{
					new (data) T(std::forward<Us>(values)...);
				}

				virtual void Destroy() override
				{
					reinterpret_cast<T*>(data)->~T();
				}
				virtual ~RefJoined() = default;
			};

			struct MakeFromFunc {};

			template<typename...>
			using void_t = void;

			template<typename T, typename = void>
			struct is_intrusively_counted
			{
				using type = std::false_type;
				static const constexpr bool value = false;
			};
			template<typename T>
			struct is_intrusively_counted<T, void_t<typename T::intrusively_counted>>
			{
				using type = std::true_type;
				static const constexpr bool value = true;
			};
		}

		template<typename, typename = void>
		class LocalSharedPtr;

		template<typename>
		class LocalWeakPtr;

		template<typename T>
		class LocalSharedPtr<T, std::enable_if_t<!detail::is_intrusively_counted<T>::value>>
		{
			detail::RefBase* ref;
			T* objRef = nullptr;

			void Increment() noexcept
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
					ref->Destroy();
					if (ref->weak_count == 0)
					{
						delete ref;
					}
					ref = nullptr;
					objRef = nullptr;
				}
			}

			void InternalReset() noexcept
			{
				InternalReset(nullptr, nullptr);
			}
			void InternalReset(T* d, detail::RefBase* rc) noexcept
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
				InternalReset(reinterpret_cast<T*>(&(reference->data)), reference);
			}
		public:
			using is_intrusively_counted = std::false_type;
			using element_type = T;

			constexpr LocalSharedPtr() noexcept
				: ref(nullptr)
				, objRef(nullptr)
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
				InternalReset(x.objRef, x.ref);
				x.ref = nullptr;
				x.objRef = nullptr;

				return *this;
			}
			~LocalSharedPtr()
			{
				Decrement();
			}

			constexpr LocalSharedPtr(std::nullptr_t p) noexcept
				: ref(nullptr)
				, objRef(nullptr)
			{}
			template<typename... Us>
			explicit LocalSharedPtr(detail::MakeFromFunc, Us&&... values)
				:LocalSharedPtr()
			{
				AllocateControlFromArgs(std::forward<Us>(values)...);
			}

			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
			explicit LocalSharedPtr(const LocalWeakPtr<U>& x)
			{
				if (x.expired()) throw std::bad_weak_ptr();

				objRef = x.refObj;
				ref = x.ref;
				Increment();
			}

			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
				explicit LocalSharedPtr(U* p)
				:LocalSharedPtr()
			{
				AllocateControl(p);
			}

			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
				LocalSharedPtr& operator=(const LocalSharedPtr<U>& x)
			{
				if (x.ref != ref)
				{
					InternalReset(x.objRef, x.ref);

					Increment();
				}
			}
			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
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

			detail::RefBase* InternalGetRef() const
			{
				return ref;
			}
			T* InternalGetPtr() const
			{
				return objRef;
			}
			template<typename U>
			void InternalSetPtr(U* ptr)
			{
				objRef = ptr;
			}
			void InternalSetPtr(std::nullptr_t)
			{
				objRef = nullptr;
			}
		};

		//The expectation of intrusively counted objects is that
		//their count starts at 1 when first constructed.
		template<typename T>
			class LocalSharedPtr<T, std::enable_if_t<detail::is_intrusively_counted<T>::value>>
		{
			T* objRef = nullptr;

			void Increment()
			{
				if (objRef == nullptr) return;
				objRef->IncrementLocalPtr();
			}
			void Decrement() noexcept
			{
				if (objRef == nullptr) return;
				objRef->DecrementLocalPtr();

				if (use_count() == 0)
				{
					delete objRef;
					objRef = nullptr;
				}
			}

			void InternalReset()
			{
				InternalReset(nullptr);
			}
			void InternalReset(T* d)
			{
				Decrement();

				objRef = d;
			}


			void AllocateControl(T* x)
			{
				InternalReset(x);
			}
			template<typename... Us>
			void AllocateControlFromArgs(Us&&... values)
			{
				InternalReset(new T(std::forward<Us>(values)...));
			}
		public:
			using is_intrusively_counted = std::true_type;
			using element_type = T;

			constexpr LocalSharedPtr() noexcept = default;
			LocalSharedPtr(const LocalSharedPtr& x)
				: objRef(x.objRef)
			{
				Increment();
			}
			LocalSharedPtr& operator=(const LocalSharedPtr& x)
			{
				if (x.objRef != objRef)
				{
					InternalReset(x.objRef);

					Increment();
				}

				return *this;
			}
			LocalSharedPtr(LocalSharedPtr&& x) noexcept
				: objRef(x.objRef)
			{
				x.objRef = nullptr;
			}
			LocalSharedPtr& operator=(LocalSharedPtr&& x) noexcept
			{
				InternalReset(x.objRef);
				x.objRef = nullptr;

				return *this;
			}
			~LocalSharedPtr()
			{
				Decrement();
			}

			constexpr LocalSharedPtr(std::nullptr_t) noexcept
				: LocalSharedPtr()
			{}
			LocalSharedPtr& operator=(std::nullptr_t)
			{
				InternalReset();
				return *this;
			}

			template<typename... Us>
			explicit LocalSharedPtr(detail::MakeFromFunc, Us&&... values)
				:LocalSharedPtr()
			{
				AllocateControlFromArgs(std::forward<Us>(values)...);
			}

			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
				explicit LocalSharedPtr(U* p)
				:LocalSharedPtr()
			{
				AllocateControl(p);
			}

			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
				LocalSharedPtr& operator=(const LocalSharedPtr<U>& x)
			{
				if (x.InternalGetPtr() != objRef)
				{
					InternalReset(x.InternalGetPtr());

					Increment();
				}
				return *this;
			}
			template<typename U,
				typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
				LocalSharedPtr& operator=(LocalSharedPtr<U>&& x)
			{
				if (x.InternalGetPtr() != objRef)
				{
					InternalReset(x.InternalGetPtr());

					x.InternalSetPtr(nullptr);
				}
				return *this;
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
				return objRef == nullptr ? 0l : long(objRef->LocalUseCount());
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
				return objRef < x.InternalGetPtr();
			}

			detail::RefBase* InternalGetRef() const
			{
				return ref;
			}
			T* InternalGetPtr() const
			{
				return objRef;
			}
			template<typename U>
			void InternalSetPtr(U* ptr)
			{
				objRef = ptr;
			}
			void InternalSetPtr(std::nullptr_t)
			{
				objRef = nullptr;
			}
		};

		template<typename T>
		class LocalWeakPtr
		{
			static_assert(!detail::is_intrusively_counted<T>::value, "Intrusively-counted objects can not support weak reference counting.");

			detail::RefBase* ref;
			T* refObj;
			friend class LocalSharedPtr<T>;

			void IncrementWeak()
			{
				if (ref == nullptr) return;
				++ref->weak_count;
			}
			void DecrementWeak()
			{
				if (ref == nullptr) return;
				--ref->weak_count;
				if (ref->weak_count == 0 && ref->ref_count == 0)
				{
					delete ref;
					ref = nullptr;
					refObj = nullptr;
				}
			}
		public:
			constexpr LocalWeakPtr() noexcept
				:ref(nullptr)
				,refObj(nullptr)
			{}
			LocalWeakPtr(const LocalWeakPtr& r)
				:ref(r.ref)
				,refObj(r.refObj)
			{
				IncrementWeak();
			}
			template<typename Y, typename = std::enable_if_t<std::is_convertible<Y*, T*>::value>>
			LocalWeakPtr(const LocalWeakPtr<Y>& r)
				:ref(r.ref)
				,refObj(r.refObj)
			{
				IncrementWeak();
			}
			template<typename Y, typename = std::enable_if_t<std::is_convertible<Y*, T*>::value>>
			LocalWeakPtr(const LocalSharedPtr<Y>& r)
				:ref(r.InternalGetRef())
				,refObj(r.get())
			{
				IncrementWeak();
			}
			LocalWeakPtr(LocalWeakPtr&& r) noexcept
				:ref(r.ref)
				,refObj(r.refObj)
			{
				r.ref = nullptr;
				r.refObj = nullptr;
			}
			template<typename Y, typename = std::enable_if_t<std::is_convertible<Y*, T*>::value>>
			LocalWeakPtr(LocalWeakPtr<Y>&& r)
				:ref(r.ref)
				,refObj(r.refObj)
			{
				r.ref = nullptr;
				r.refObj = nullptr;
			}
			~LocalWeakPtr()
			{
				DecrementWeak();
			}

			LocalWeakPtr& operator=(const LocalWeakPtr& r)
			{
				if (r.ref != ref)
				{
					if (ref != nullptr)
					{
						DecrementWeak();
					}

					ref = r.ref;
					refObj = r.refObj;

					if (ref != nullptr)
					{
						IncrementWeak();
					}
				}
				return *this;
			}
			template<typename Y, typename = std::enable_if_t<std::is_assignable<Y*, T*>::value>>
			LocalWeakPtr& operator=(const LocalWeakPtr<Y>& r)
			{
				if (ref != r.ref)
				{
					if (ref != nullptr)
					{
						DecrementWeak();
					}

					ref = r.ref;
					refObj = r.refObj;

					if (ref != nullptr)
					{
						IncrementWeak();
					}
				}
				return *this;
			}
			template<typename Y, typename = std::enable_if_t<std::is_assignable<Y*, T*>::value>>
			LocalWeakPtr& operator=(const LocalSharedPtr<Y>& r)
			{
				if (ref != r.ref)
				{
					if (ref != nullptr)
					{
						DecrementWeak();
					}

					ref = r.ref;
					refObj = r.objRef;

					if (ref != nullptr)
					{
						IncrementWeak();
					}
				}
				return *this;
			}
			LocalWeakPtr& operator=(LocalWeakPtr&& r) noexcept
			{
				if (ref != r.ref)
				{
					if (ref != nullptr)
					{
						DecrementWeak();
					}

					ref = r.ref;
					refObj = r.refObj;

					r.ref = nullptr;
					r.refObj = nullptr;
				}
				return *this;
			}
			template<typename Y, typename = std::enable_if_t<std::is_assignable<Y*, T*>::value>>
			LocalWeakPtr& operator=(LocalWeakPtr<Y>&& r) noexcept
			{
				if (ref != r.ref)
				{
					if (ref != nullptr)
					{
						DecrementWeak();
					}

					ref = r.ref;
					refObj = r.refObj;

					r.ref = nullptr;
					r.refObj = nullptr;
				}
				return *this;
			}

			T* InternalGetPtr() const
			{
				return refObj;
			}
			detail::RefBase* InternalGetRef() const
			{
				return ref;
			}

			void reset()
			{
				DecrementWeak();

				ref = nullptr;
				refObj = nullptr;
			}
			void swap(LocalWeakPtr& r) noexcept
			{
				using std::swap;

				swap(ref, r.ref);
				swap(refObj, r.refObj);
			}
			long use_count() const
			{
				return ref == nullptr ? 0 : ref->ref_count;
			}
			bool expired() const
			{
				return use_count() == 0;
			}
			LocalSharedPtr<T> lock() const
			{
				return expired() ? LocalSharedPtr<T>() : LocalSharedPtr<T>(*this);
			}
			template<typename U>
			bool owner_before(const LocalSharedPtr<U>& x)
			{
				return ref < x.InternalGetRef();
			}
		};

		template<typename U1, typename U2>
		bool operator==(const LocalSharedPtr<U1>& x,
			const LocalSharedPtr<U2>& y) noexcept
		{
			return x.get() == y.get();
		}
		template<typename U1>
		bool operator==(const LocalSharedPtr<U1>& x,
			std::nullptr_t npt) noexcept
		{
			return x.get() == npt;
		}
		template<typename U1>
		bool operator==(std::nullptr_t npt,
			const LocalSharedPtr<U1>& x) noexcept
		{
			return x == npt;
		}

		template<typename U1, typename U2>
		bool operator!=(const LocalSharedPtr<U1>& x,
			const LocalSharedPtr<U2>& y) noexcept
		{
			return !(x == y);
		}
		template<typename U1>
		bool operator!=(const LocalSharedPtr<U1>& x,
			std::nullptr_t npt) noexcept
		{
			return !(x == npt);
		}
		template<typename U1>
		bool operator!=(std::nullptr_t npt,
			const LocalSharedPtr<U1>& x) noexcept
		{
			return x != npt;
		}

		template<typename U1, typename U2>
		bool operator<(const LocalSharedPtr<U1>& x,
			const LocalSharedPtr<U2>& y) noexcept
		{
			return std::less<
				std::common_type_t<decltype(x.get()),
				decltype(y.get())>>()(x.get(), y.get());
		}
		template<typename U1>
		bool operator<(const LocalSharedPtr<U1>& x,
			std::nullptr_t npt) noexcept
		{
			return std::less<U1*>()(x.get(), (U1*)npt);
		}
		template<typename U1>
		bool operator<(std::nullptr_t npt,
			const LocalSharedPtr<U1>& y) noexcept
		{
			return std::less<U1*>()((U1*)npt, y.get());
		}

		template<typename U1, typename U2>
		bool operator<=(const LocalSharedPtr<U1>& x,
			const LocalSharedPtr<U2>& y) noexcept
		{
			return !(y < x);
		}
		template<typename U1>
		bool operator<=(const LocalSharedPtr<U1>& x,
			std::nullptr_t npt) noexcept
		{
			return !(npt < x.get());
		}
		template<typename U1>
		bool operator<=(std::nullptr_t npt,
			const LocalSharedPtr<U1>& y) noexcept
		{
			return !(y.get() < npt);
		}

		template<typename U1, typename U2>
		bool operator>(const LocalSharedPtr<U1>& x,
			const LocalSharedPtr<U2>& y) noexcept
		{
			return y < x;
		}
		template<typename U1>
		bool operator>(const LocalSharedPtr<U1>& x,
			std::nullptr_t npt) noexcept
		{
			return npt < x;
		}
		template<typename U1>
		bool operator>(std::nullptr_t npt,
			const LocalSharedPtr<U1>& y) noexcept
		{
			return y < npt;
		}

		template<typename U1, typename U2>
		bool operator>=(const LocalSharedPtr<U1>& x,
			const LocalSharedPtr<U2>& y) noexcept
		{
			return !(x < y);
		}
		template<typename U1>
		bool operator>=(const LocalSharedPtr<U1>& x,
			std::nullptr_t npt) noexcept
		{
			return !(x < npt);
		}
		template<typename U1>
		bool operator>=(std::nullptr_t npt,
			const LocalSharedPtr<U1>& y) noexcept
		{
			return !(npt < y);
		}

		template<typename U1, typename U2>
		void swap(LocalSharedPtr<U1>& x, LocalSharedPtr<U2>& y) noexcept
		{
			using std::swap;

			swap(x.ref, y.ref);
			swap(x.objRef, y.objRef);
		}

		template<typename T, typename... Us>
		LocalSharedPtr<T> make_localshared(Us&&... values)
		{
			return LocalSharedPtr<T>(detail::MakeFromFunc(),
				std::forward<Us>(values)...);
		}
		void TestingLocalSharedPtr();

	}
}