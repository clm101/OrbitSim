#include "Random.h"
#include <limits>

namespace clm {
	float getrandom() noexcept {
		static std::random_device r;

		return (float)r() / (float)std::numeric_limits<unsigned int>::max();
	}
}