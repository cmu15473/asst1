#include "Debug.h"
#include <Windows.h>
namespace CoreLib
{
	namespace Diagnostics
	{
		void Debug::Write(const String & text)
		{
	#ifdef _DEBUG
			if (IsDebuggerPresent() != 0)
			{
				OutputDebugString(text.Buffer());
			}
	#endif
		}

		void Debug::WriteLine(const String & text)
		{
	#ifdef _DEBUG
			if (IsDebuggerPresent() != 0)
			{
				OutputDebugString(text.Buffer());
				OutputDebugString(L"\n");
			}
	#endif
		}
	}
}