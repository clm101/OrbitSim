#ifndef TIMER_H
#define TIMER_H

#include "DebugDefines/MemoryDebug.h"
#include <chrono>

namespace clm {
	class Timer {
	private:
		double fInitial;
	public:
		Timer();
		Timer(const Timer&) = delete;
		Timer& operator=(const Timer&) = delete;

		const double peek() noexcept;
		void set() noexcept;
	};
}

#endif