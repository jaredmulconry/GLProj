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

		}
	}
}