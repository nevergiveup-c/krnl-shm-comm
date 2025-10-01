#pragma once

using hash_t = unsigned long long;

template<size_t N>
__forceinline consteval hash_t HASH_COMPILE_TIME(char const (&data)[N])
{
	hash_t hash = 0;

	for (auto i = 0; i < N - 1; ++i)
	{
		hash += data[i] >= 'A' && data[i] <= 'Z' ? data[i] + ('a' - 'A') : data[i];
		hash += hash << 8;
		hash ^= hash >> 11;
	}

	hash += hash << 5;
	hash ^= hash >> 13;
	hash += hash << 10;

	return hash;
}

__forceinline hash_t HASH_RUNTIME(char const* str)
{
	size_t length = 0;
	while (str[length])
		++length;

	hash_t hash = 0;

	for (auto i = 0u; i < length; i++)
	{
		hash += str[i] >= 'A' && str[i] <= 'Z' ? str[i] + ('a' - 'A') : str[i];
		hash += hash << 8;
		hash ^= hash >> 11;
	}

	hash += hash << 5;
	hash ^= hash >> 13;
	hash += hash << 10;

	return hash;
}

__forceinline hash_t HASH_RUNTIME_W(const wchar_t* str)
{
	size_t length = 0;
	while (str[length])
		++length;

	hash_t hash = 0;
	for (auto i = 0u; i < length; i++)
	{
		wchar_t ch = str[i];
		char narrowChar = (ch < 128) ? static_cast<char>(ch) : '?';
		hash += narrowChar >= 'A' && narrowChar <= 'Z' ? narrowChar + ('a' - 'A') : narrowChar;
		hash += hash << 8;
		hash ^= hash >> 11;
	}

	hash += hash << 5;
	hash ^= hash >> 13;
	hash += hash << 10;

	return hash;
}

#define HASH( s )  HASH_COMPILE_TIME( s  )
#define HASH_RT( s ) HASH_RUNTIME( s )
#define HASH_RTW( s ) HASH_RUNTIME_W( s )