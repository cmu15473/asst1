#ifndef CORE_LIB_THREADING_H
#define CORE_LIB_THREADING_H
#include <atomic>
#include <thread>
#include "Basic.h"

namespace CoreLib
{
	namespace Threading
	{
		class SpinLock
		{
		private:
			std::atomic<long> lock;
		public:
			SpinLock()
			{
				lock = 0;
			}
			inline bool TryLock()
			{
				return (lock.exchange(1, std::memory_order_relaxed) == 0);
			}
			inline void Lock()
			{
				while (lock.exchange(1, std::memory_order_relaxed) != 0);
			}
			inline void Unlock()
			{
				lock.exchange(0, std::memory_order_relaxed);
			}
		};
	}
}

#endif
