#ifndef CLM_TIMER_H
#define CLM_TIMER_H

#include "DebugDefines/MemoryDebug.h"
#include <chrono>

class clmTimer {
private:
	double fInitial;
public:
	clmTimer();
	clmTimer(const clmTimer&) = delete;
	clmTimer& operator=(const clmTimer&) = delete;

	const double peek() noexcept;
	void set() noexcept;
};

#endif