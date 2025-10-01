#pragma once
#include <cstdint>

enum class eLogLevel : uint8_t
{
	none,
	info,
	warning,
	critical,
	dev,
};