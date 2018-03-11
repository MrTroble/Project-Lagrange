#pragma once

#include <cstdint>

#define PI 3.14159265358979323846
#define PIx2 6.28318530717958647692

namespace math {

	float f_max(float val1, float val2);

	int i_max(int val1, int val2);

	float f_min(float val1, float val2);

	int i_min(int val1, int val2);

	std::uint32_t u_max(std::uint32_t val1, std::uint32_t val2);

	std::uint32_t u_min(std::uint32_t val1, std::uint32_t val2);

}