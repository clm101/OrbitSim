#include "Timer.h"

namespace clm {
	Timer::Timer() {
		fInitial = ((std::chrono::duration<double>)std::chrono::system_clock::now().time_since_epoch()).count();
	}

	const double Timer::peek() noexcept {
		double fTimeTmp = ((std::chrono::duration<double>)std::chrono::system_clock::now().time_since_epoch()).count();
		return (fTimeTmp - fInitial);
	}

	void Timer::set() noexcept {
		fInitial = ((std::chrono::duration<double>)std::chrono::system_clock::now().time_since_epoch()).count();
	}
}