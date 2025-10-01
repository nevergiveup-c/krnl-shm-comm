#pragma once
#include <windows.h>

#include <string>

#include "../shared/hash.h"

namespace shared
{
	std::wstring getCurrentAppFolder();

	int wcharToChar(int wideChar, char* charArray);

	size_t wcharArrayToCharArray(const wchar_t* arr, char* out);

	void charArrayToWcharArray(const char* in, wchar_t* out);

	HANDLE getProcessIdByName(hash_t processHash);
}
