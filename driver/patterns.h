#pragma once
#include <ntdef.h>

#include "handle.h"

namespace memory::scanner
{
	constexpr size_t maxPatternBytes = 512;

	class pattern
	{
	public:
		explicit pattern(const char* idaSig);

		UCHAR bytes[maxPatternBytes];
		size_t size;
	};

	handle searchPattern(const char* sig, uintptr_t moduleStart, size_t moduleSize);
}
