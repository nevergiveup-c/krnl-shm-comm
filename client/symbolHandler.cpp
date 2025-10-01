#include "symbolHandler.h"

#include <windows.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "urlmon.lib")

SymbolHandler::SymbolHandler(const std::wstring& path) : cachePath(path),
process(GetCurrentProcess())
{
	SymInitializeW(process, (L"cache*" + path +
		L";SRV*http://msdl.microsoft.com/download/symbols").c_str(), FALSE);

	SymSetOptions(SYMOPT_EXACT_SYMBOLS | SYMOPT_DEBUG);
}

SymbolHandler::~SymbolHandler()
{
	SymCleanup(process);
}

std::wstring SymbolHandler::getPdb(const std::wstring& binaryPath) const
{
	SYMSRV_INDEX_INFOW info{};
	info.sizeofstruct = sizeof(SYMSRV_INDEX_INFOW);

	if (auto const result = SymSrvGetFileIndexInfoW(binaryPath.c_str(), &info, 0); !result)
	{
		std::wcout << "[-] Failed to find binary info. Error: " << GetLastError() << std::endl;
		return {};
	}

	// Format identifier
	std::wstring identifier;
	if (info.guid != GUID{})
	{
		wchar_t buffer[42];
		swprintf_s(buffer, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%X",
			info.guid.Data1, info.guid.Data2, info.guid.Data3,
			info.guid.Data4[0], info.guid.Data4[1], info.guid.Data4[2], info.guid.Data4[3],
			info.guid.Data4[4], info.guid.Data4[5], info.guid.Data4[6], info.guid.Data4[7],
			info.age);
		identifier = buffer;
	}
	else
	{
		wchar_t buffer[17];
		swprintf_s(buffer, L"%08X%X", info.sig, info.age);
		identifier = buffer;
	}

	std::wstring pdbFileName = info.pdbfile;
	std::wstring localPath = cachePath + L"\\" + pdbFileName + L"\\" + identifier + L"\\" + pdbFileName;

	// Check if already exists
	if (std::filesystem::exists(localPath))
	{
		return localPath;
	}

	// Create directory and download
	std::filesystem::create_directories(std::filesystem::path(localPath).parent_path());
	std::wstring downloadUrl = L"https://msdl.microsoft.com/download/symbols/" + pdbFileName + L"/" + identifier + L"/" + pdbFileName;

	std::wcout << L"[+] Downloading: " << downloadUrl << std::endl;

	if (auto const hr = URLDownloadToFileW(nullptr, downloadUrl.c_str(), localPath.c_str(), 0, nullptr);
		SUCCEEDED(hr) && std::filesystem::exists(localPath))
	{
		std::wcout << L"[+] Downloaded: " << localPath << std::endl;
		return localPath;
	}

	std::wcout << L"[-] Download failed" << std::endl;
	return {};
}

std::vector<std::uint64_t> SymbolHandler::getOffset(const std::wstring& pdbPath, const std::vector<std::wstring>& symbolName) const
{
	DWORD64 base = 0x40000;
	DWORD size = static_cast<DWORD>(std::filesystem::file_size(pdbPath));

	// Load symbols for the module 
	std::wcout << "[+] Loading Symbols From " << pdbPath << std::endl;
	DWORD64 ModBase = SymLoadModuleExW(
		process, // Process handle of the current process 
		nullptr,                // Handle to the module's image file (not needed)
		pdbPath.c_str(),           // Path/name of the file 
		nullptr,                // User-defined short name of the module (it can be NULL) 
		base,            // Base address of the module (cannot be NULL if .PDB file is used, otherwise it can be NULL) 
		size,            // Size of the file (cannot be NULL if .PDB file is used, otherwise it can be NULL) 
		nullptr,
		NULL
	);

	if (ModBase == 0)
	{
		std::wcout << "[-] Error: SymLoadModule64() failed. Error code: " << GetLastError() << std::endl;
		return {};
	}

	std::vector<std::uint64_t> offsets{};

	for (const auto& sym : symbolName)
	{
		// Check if this is a class field (contains dot) or regular symbol
		size_t dotPos = sym.find(L'.');

		if (dotPos != std::wstring::npos)
		{
			// Handle class field - logic from GetClassOffset
			std::wstring className = sym.substr(0, dotPos);
			std::wstring fieldName = sym.substr(dotPos + 1);

			if (className.empty() || fieldName.empty())
			{
				std::wcout << "[-] Error: Invalid symbol format " << sym << ". Both class name and field name must be non-empty" << std::endl;
				offsets.push_back(EMPTY);
				continue;
			}

			// Convert wide strings to narrow strings for API calls
			std::string classNameA;
			std::string fieldNameA;

			// Safe conversion using WideCharToMultiByte
			int classNameSize = WideCharToMultiByte(CP_UTF8, 0, className.c_str(), -1, nullptr, 0, nullptr, nullptr);

			if (classNameSize > 0)
			{
				classNameA.resize(classNameSize - 1);
				WideCharToMultiByte(CP_UTF8, 0, className.c_str(), -1, classNameA.data(), classNameSize, nullptr, nullptr);
			}

			int fieldNameSize = WideCharToMultiByte(CP_UTF8, 0, fieldName.c_str(), -1, nullptr, 0, nullptr, nullptr);

			if (fieldNameSize > 0)
			{
				fieldNameA.resize(fieldNameSize - 1);
				WideCharToMultiByte(CP_UTF8, 0, fieldName.c_str(), -1, fieldNameA.data(), fieldNameSize, nullptr, nullptr);
			}

			SYMBOL_INFO symbol{};
			symbol.SizeOfStruct = sizeof(SYMBOL_INFO);

			// Get the class type information
			BOOL bRet = SymGetTypeFromName(process, ModBase, classNameA.c_str(), &symbol);

			if (!bRet || !symbol.TypeIndex)
			{
				std::wcout << "[-] Error: SymGetTypeFromName() failed for class " << className << " in symbol " << sym << ". Error code: " << GetLastError() << std::endl;
				offsets.push_back(EMPTY);
				continue;
			}

			DWORD numChildren = 0;

			if (!SymGetTypeInfo(process, ModBase, symbol.TypeIndex, TI_GET_CHILDRENCOUNT, &numChildren) || numChildren == 0)
			{
				std::wcout << "[-] Error: Failed to get children count for class " << className << " in symbol " << sym << ". Error code: " << GetLastError() << std::endl;
				offsets.push_back(EMPTY);
				continue;
			}

			// Allocate buffer for children information with proper size
			size_t bufferSize = sizeof(TI_FINDCHILDREN_PARAMS) + (numChildren - 1) * sizeof(ULONG);
			std::vector<BYTE> buffer(bufferSize);
			TI_FINDCHILDREN_PARAMS* params = reinterpret_cast<TI_FINDCHILDREN_PARAMS*>(buffer.data());
			params->Count = numChildren;
			params->Start = 0;

			if (!SymGetTypeInfo(process, ModBase, symbol.TypeIndex, TI_FINDCHILDREN, params))
			{
				std::wcout << "[-] Error: Failed to get children for class " << className << " in symbol " << sym << ". Error code: " << GetLastError() << std::endl;
				offsets.push_back(EMPTY);
				continue;
			}

			// Search for the specific field
			bool fieldFound = false;
			DWORD fieldOffset = 0;

			for (DWORD j = 0; j < numChildren; ++j)
			{
				ULONG childId = params->ChildId[j];
				WCHAR* memberName = nullptr;

				if (!SymGetTypeInfo(process, ModBase, childId, TI_GET_SYMNAME, &memberName) || !memberName)
				{
					continue;
				}

				// Convert member name to narrow string for comparison
				std::string memberNameA;
				int memberNameSize = WideCharToMultiByte(CP_UTF8, 0, memberName, -1, nullptr, 0, nullptr, nullptr);

				if (memberNameSize > 0)
				{
					memberNameA.resize(memberNameSize - 1);
					WideCharToMultiByte(CP_UTF8, 0, memberName, -1, &memberNameA[0], memberNameSize, nullptr, nullptr);
				}

				if (memberNameA == fieldNameA)
				{
					if (SymGetTypeInfo(process, ModBase, childId, TI_GET_OFFSET, &fieldOffset))
					{
						fieldFound = true;
						LocalFree(memberName);
						break;
					}
					else
					{
						std::wcout << "[-] Error: Failed to get offset for field " << fieldName
							<< " in class " << className << " for symbol " << sym << ". Error code: " << GetLastError() << std::endl;
						offsets.push_back(EMPTY);
						LocalFree(memberName);
						continue;
					}
				}

				LocalFree(memberName);
			}

			if (!fieldFound) {
				std::wcout << "[-] Error: Field " << fieldName << " not found in class " << className << " for symbol " << sym << std::endl;
				offsets.push_back(EMPTY);
				continue;
			}

			// Display information about the symbol 
			std::wcout << "[+] Symbol " << sym << " Offset: " << fieldOffset << std::endl;
			offsets.push_back(fieldOffset);
		}
		else {
			// Handle regular function/symbol - logic from GetFunctionOffset
			SYMBOL_INFO_PACKAGEW SymInfoPackage{};
			SymInfoPackage.si.SizeOfStruct = sizeof(SYMBOL_INFOW);
			SymInfoPackage.si.MaxNameLen = sizeof(SymInfoPackage.name);

			BOOL bRet = SymFromNameW(
				process,
				sym.c_str(),
				&SymInfoPackage.si
			);
			if (!bRet || !SymInfoPackage.si.Address)
			{
				std::wcout << "[-] Error: SymFromName() failed. Sym: " << sym << " || Error code: " << GetLastError() << std::endl;
				offsets.push_back(EMPTY);
				continue;
			}

			// Display information about the symbol 
			std::wcout << "[+] Symbol " << sym << " Offset: " << static_cast<DWORD>(SymInfoPackage.si.Address - ModBase) << std::endl;
			offsets.push_back(SymInfoPackage.si.Address - ModBase);
		}
	}

	SymUnloadModule64(GetCurrentProcess(), ModBase);
	return offsets;
}
