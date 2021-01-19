#include "clmTimer.h"

clmTimer::clmTimer() {
	fInitial = ((std::chrono::duration<double>)std::chrono::system_clock::now().time_since_epoch()).count();
}

const double clmTimer::peek() noexcept {
	double fTimeTmp = ((std::chrono::duration<double>)std::chrono::system_clock::now().time_since_epoch()).count();
	return (fTimeTmp - fInitial);
}

void clmTimer::set() noexcept {
	fInitial = ((std::chrono::duration<double>)std::chrono::system_clock::now().time_since_epoch()).count();
}