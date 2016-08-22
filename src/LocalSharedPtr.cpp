#include "LocalSharedPtr.hpp"

///How to build an intrusively-counted LocalSharedPtr
class SpecialClass
{
	using intrusively_counted = void;
	long counter = 0;
public:
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

namespace GlProj
{
	namespace Utilities
	{
		template<typename T>
		void TestPtrType(LocalSharedPtr<T>& x, T value)
		{
			auto y = make_localshared<T>(value);
			(void)(x != y);

			y = x;
			y = std::move(x);
			x = make_localshared<T>();
			auto ptr = x.get();
			*ptr = value;

			x >= y;
			x <= nullptr;
			nullptr > y;
		}

		void TestingLocalSharedPtr()
		{
			LocalSharedPtr<int> t1(new int(42));
			
			TestPtrType(t1, 1729);

			auto magic = GlProj::Utilities::make_localshared<SpecialClass>();
			TestPtrType(magic, SpecialClass());
			auto copiedMagic = magic;
		}
	}
}