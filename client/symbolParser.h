#pragma once
#include <string>
#include <vector>

#include "symbolHandler.h"

class SymbolParser
{
	std::vector<std::wstring> targetBinaries =
	{
		L"C:\\Windows\\System32\\ntoskrnl.exe",
	};

	std::vector<Sym> symbolsToRetrieve =
	{
		{ L"ntoskrnl.exe", L"_KTHREAD.MiscFlags" },
		{ L"ntoskrnl.exe", L"_KTHREAD.ThreadFlags" },
		{ L"ntoskrnl.exe", L"_ETHREAD.ThreadListEntry" },
		{ L"ntoskrnl.exe", L"_ETHREAD.StartAddress" },
		{ L"ntoskrnl.exe", L"_ETHREAD.Win32StartAddress" },
		{ L"ntoskrnl.exe", L"_EPROCESS.ObjectTable" },
		{ L"ntoskrnl.exe", L"_EPROCESS.ThreadListHead" },
		{ L"ntoskrnl.exe", L"_EPROCESS.ActiveThreads" },
		{ L"ntoskrnl.exe", L"_EPROCESS.VadRoot" },
		{ L"ntoskrnl.exe", L"_EPROCESS.VadHint" },
		{ L"ntoskrnl.exe", L"_EPROCESS.VadCount" },
		{ L"ntoskrnl.exe", L"_EPROCESS.Peb" },
		{ L"ntoskrnl.exe", L"_ETHREAD.Cid" },
		{ L"ntoskrnl.exe", L"PspCidTable" },
		{ L"ntoskrnl.exe", L"ObpKernelHandleTable" },
		{ L"ntoskrnl.exe", L"ObpObjectTypes" },
		{ L"ntoskrnl.exe", L"MmSectionObjectType" },
		{ L"ntoskrnl.exe", L"ObHeaderCookie" },
		{ L"ntoskrnl.exe", L"ObpSymbolicLinkObjectType" },
		{ L"ntoskrnl.exe", L"ObpRootDirectoryObject" },
		{ L"ntoskrnl.exe", L"ObpInfoMaskToOffset" },
		{ L"ntoskrnl.exe", L"EtwpLogger" },
		{ L"ntoskrnl.exe", L"KeGetCurrentPrcb" },
	};

public:
	SymbolParser() = default;
	SymbolParser(SymbolParser const&) = delete;
	SymbolParser& operator=(SymbolParser const&) = delete;
	SymbolParser(SymbolParser&&) = delete;
	SymbolParser& operator=(SymbolParser&&) = delete;
	~SymbolParser() = default;

	[[nodiscard]] std::vector<std::uint64_t> parse() const;

	static SymbolParser& instance()
	{
		static SymbolParser _instance{};
		return _instance;
	}
};
