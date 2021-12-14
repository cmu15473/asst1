#ifndef GX_WIN_DEBUG_H
#define GX_WIN_DEBUG_H

#include "../Basic.h"

namespace CoreLib
{
	namespace Diagnostics
	{
		using namespace CoreLib::Basic;
		class Debug
		{
		public:
			static void Write(const String & text);
			static void WriteLine(const String & text);
		};
	}
}

#endif