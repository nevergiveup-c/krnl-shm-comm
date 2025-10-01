
#include "symbolParser.h"

#include "shared.hpp"

std::vector<std::uint64_t> SymbolParser::parse() const
{
	const SymbolHandler handler(shared::getCurrentAppFolder() + L"\\Symbols");

	std::vector<std::uint64_t> out{};

	for (const auto& binPath : targetBinaries)
	{
		auto pdbPath = handler.getPdb(binPath);

		if (pdbPath.empty())
		{
			return{};
		}

		std::vector<std::wstring> symbolsForThisFile{};

		for (const auto& [binaryName, symbolName] : symbolsToRetrieve)
		{
			if (binPath.find(binaryName) == std::wstring::npos)
			{
				continue;
			}
			symbolsForThisFile.push_back(symbolName);
		}

		auto offsets = handler.getOffset(pdbPath, symbolsForThisFile);

		if (offsets.size() != symbolsForThisFile.size())
		{
			return{};
		}

		for (auto v : offsets)
		{
			out.push_back(v);
		}
	}

	return out;
}
