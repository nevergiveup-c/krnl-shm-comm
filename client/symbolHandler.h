#pragma once

#include <windows.h>
#include <string>
#include <vector>

struct Sym
{
	std::wstring binaryName;
	std::wstring symbolName;
};

class SymbolHandler
{
	auto static constexpr EMPTY = 0xDEADBEEF;

public:
	SymbolHandler(const std::wstring& path);
	~SymbolHandler();

	[[nodiscard]] std::wstring getPdb(const std::wstring& binaryPath) const;
	[[nodiscard]] std::vector<std::uint64_t> getOffset(const std::wstring& pdbPath,
		const std::vector<std::wstring>& symbolName) const;

private:
	std::wstring cachePath;
	HANDLE process;
};