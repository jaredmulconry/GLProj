#include "LocalSharedPtr.hpp"

///How to build an intrusively-counted LocalSharedPtr
class SpecialClass
{
	using intrusively_counted = void;
	long counter = 1;
public:
	virtual ~SpecialClass() = default;

	///Required for Increments
	void IncrementLocalPtr() noexcept
	{
		++counter;
	}
	///Required for Decrements
	void DecrementLocalPtr() noexcept
	{
		--counter;
	}
	///Required for access to the use count
	long LocalUseCount() const noexcept
	{
		return counter;
	}
};

class DerivedSpecialClass : public SpecialClass
{

};

namespace GlProj
{
	namespace Utilities
	{
		template<typename T, typename U>
		void TestPtrType(LocalSharedPtr<T>& x, T value, U* ptrToRelated)
		{
			//Construction and Assignment
			auto c1 = LocalSharedPtr<T>();
			auto c2 = c1;
			auto c3 = make_localshared<T>(value);
			c1 = c3;
			auto c4 = std::move(c3);
			c2 = std::move(c4);
			auto c5 = nullptr;
			c3 = nullptr;
			auto c6 = LocalSharedPtr<T>(ptrToRelated);
			auto c7 = LocalSharedPtr<U>();
			c6 = c7;
			c3 = std::move(c7);
			//

			auto y = make_localshared<T>(value);
			(void)(x != y);

			y = x;
			y = std::move(x);
			x = make_localshared<T>();
			auto ptr = x.get();
			*ptr = value;
			*y = value;

			auto tmp = y;
			y.use_count();
			y.unique();
			if (y)
			{
				y.reset();
			}
			x.reset(new T(value));

			y = make_localshared<T>();

			x >= y;
			x <= nullptr;
			nullptr > y;

			x.owner_before(y);
		}

		void TestingLocalSharedPtr()
		{
			LocalSharedPtr<int> t1(new int(42));
			TestPtrType(t1, 1729, new int(1234));

			auto magic = GlProj::Utilities::make_localshared<SpecialClass>();
			TestPtrType(magic, SpecialClass(), new DerivedSpecialClass);
			auto copiedMagic = magic;
		}
	}
}