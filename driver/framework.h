#pragma once

#define LOG(format, ...) \
   DbgPrint("[%s:%u]: " format "", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#include <cstdint>

using byte = uint8_t;
using max_align_t = double;

#include <type_traits>

template <typename r, typename ... types>
constexpr std::integral_constant<unsigned, sizeof ...(types)> getArgumentCount(r(*f)(types ...))
{
	return std::integral_constant<unsigned, sizeof ...(types)>{};
}
