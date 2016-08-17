#include "LocalSharedPtr.hpp"

namespace GlProj
{
	namespace Utilities
	{
		void TestingLocalSharedPtr()
		{
			LocalSharedPtr<int> t1(new int(42));
			auto t2 = make_localshared<int>(1729);

			(void)(t1 != t2);

			t2 = t1;
			t2 = std::move(t1);
			t1 = make_localshared<int>();
			auto ptr = t1.get();
			*ptr = 321;

			t1 >= t2;
			t1 <= nullptr;
			nullptr > t2;
		}
	}
}