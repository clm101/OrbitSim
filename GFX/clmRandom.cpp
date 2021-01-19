#include "clmRandom.h"
#include <limits>

float getrandom() noexcept {
	static std::random_device r;

	return (float)r() / (float)std::numeric_limits<unsigned int>::max();
}