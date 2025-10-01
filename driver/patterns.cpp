#include "patterns.h"

#include "pair.h"

namespace memory::scanner
{
	static pair<uint8_t, bool> toHex(const char c)
	{
		switch (c)
		{
		case '0': return { 0x0, true };
		case '1': return { 0x1, true };
		case '2': return { 0x2, true };
		case '3': return { 0x3, true };
		case '4': return { 0x4, true };
		case '5': return { 0x5, true };
		case '6': return { 0x6, true };
		case '7': return { 0x7, true };
		case '8': return { 0x8, true };
		case '9': return { 0x9, true };
		case 'a': return { 0xa, true };
		case 'b': return { 0xb, true };
		case 'c': return { 0xc, true };
		case 'd': return { 0xd, true };
		case 'e': return { 0xe, true };
		case 'f': return { 0xf, true };
		case 'A': return { 0xA, true };
		case 'B': return { 0xB, true };
		case 'C': return { 0xC, true };
		case 'D': return { 0xD, true };
		case 'E': return { 0xE, true };
		case 'F': return { 0xF, true };
		default:  return { 0, false };
		}
	}

	pattern::pattern(const char* idaSig) : size(0)
	{
		const size_t len = strlen(idaSig);
		if (len < 2)
		{
			return;
		}

		// We subtract one to avoid overrunning when processing pairs.
		const size_t sizeMinusOne = len - 1;

		// Loop through the signature string.
		for (size_t i = 0; i < sizeMinusOne && size < maxPatternBytes; ++i)
		{
			// Skip spaces.

			if (idaSig[i] == ' ')
			{
				continue;
			}

			// If the character is not a wildcard, process two hex digits.
			if (idaSig[i] != '?')
			{
				const auto c1 = toHex(idaSig[i]);
				const auto c2 = toHex(idaSig[i + 1]);
				if (c1.m_second && c2.m_second)
				{
					// Combine two hex digits into one byte.
					bytes[size++] = static_cast<uint8_t>((c1.m_first * 0x10) + c2.m_first);
				}
				// Skip the next character since we already processed it.
				++i;
			}
			else
			{
				// For a wildcard, store a placeholder value (0 in this case).
				bytes[size++] = 0;
			}
		}
	}

	static handle scanPattern(const uint8_t* sig, const std::uint64_t length, handle begin, const std::uint64_t module_size)
	{
		std::uint64_t maxShift = length;
		const std::uint64_t maxIdx = length - 1;

		//Get wildcard index, and store max shiftable byte count
		std::uint64_t wildCardIdx{ static_cast<size_t>(-1) };
		for (int i{ static_cast<int>(maxIdx - 1) }; i >= 0; --i)
		{
			if (!sig[i])
			{
				maxShift = maxIdx - i;
				wildCardIdx = i;
				break;
			}
		}

		//Store max shiftable bytes for non wildcards.
		std::uint64_t shiftTable[UINT8_MAX + 1]{};
		for (std::uint64_t i{}; i <= UINT8_MAX; ++i)
		{
			shiftTable[i] = maxShift;
		}

		//Fill shift table with sig bytes
		for (std::uint64_t i{ wildCardIdx + 1 }; i != maxIdx; ++i)
		{
			shiftTable[sig[i]] = maxIdx - i;
		}

		//Loop data
		const auto scanEnd = module_size - length;
		for (std::uint64_t currentIdx{}; currentIdx <= scanEnd;)
		{
			for (std::int64_t sigIdx{ static_cast<std::int64_t>(maxIdx) }; sigIdx >= 0; --sigIdx)
			{
				if (sig[sigIdx] && *begin.add(currentIdx + sigIdx).as<uint8_t*>() != sig[sigIdx])
				{
					currentIdx += shiftTable[*begin.add(currentIdx + maxIdx).as<uint8_t*>()];
					break;
				}

				if (sigIdx == NULL)
				{
					return begin.add(currentIdx);
				}
			}
		}
		return nullptr;
	}

	handle searchPattern(const char* sig, const uintptr_t moduleStart, const size_t moduleSize)
	{
		const auto pat = pattern(sig);

		const auto data = pat.bytes;
		const auto length = pat.size;

		if (const auto result = scanPattern(data, length, handle(moduleStart), moduleSize); result)
		{
			return result;
		}

		return nullptr;
	}
}
