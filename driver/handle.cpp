#include <ntddk.h>
#include <ntimage.h>
#include <wdm.h>

#include "handle.h"
#include "ntclasses.h"
#include "symbols.h"

namespace memory
{
	handle handle::getModule(PVOID process, hash_t moduleHash)
	{
		auto const peb = *symbols->get<_PEB*, &NT_OFFSETS::epPeb>(process);

		auto const ldr = *reinterpret_cast<_PEB_LDR_DATA**>(reinterpret_cast<UCHAR*>(peb) + 0x18);

		if (!MmIsAddressValid(ldr))
		{
			return {};
		}

		PLIST_ENTRY head = &ldr->InLoadOrderModuleList;
		PLIST_ENTRY current = head->Flink;

		while (current != head && current != nullptr)
		{
			auto const entry = CONTAINING_RECORD(current, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			if (entry->BaseDllName.Buffer && entry->BaseDllName.Length > 0)
			{
				ANSI_STRING ansiName;

				NTSTATUS status = RtlUnicodeStringToAnsiString(&ansiName, &entry->BaseDllName,
					TRUE);

				if (NT_SUCCESS(status))
				{
					if (HASH_RUNTIME(ansiName.Buffer) == moduleHash)
					{
						const handle h{ entry->DllBase };
						RtlFreeAnsiString(&ansiName);
						return h;
					}

					RtlFreeAnsiString(&ansiName);
				}
			}

			current = current->Flink;
		}

		return {};
	}

	handle handle::getProc(ULONG64 base, hash_t procHash)
	{
		if (!MmIsAddressValid(reinterpret_cast<PVOID>(base)))
		{
			return {};
		}

		auto const begin = reinterpret_cast<UCHAR*>(base);

		const auto pe = reinterpret_cast<PIMAGE_DOS_HEADER>(base);

		if (pe->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return {};
		}

		const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS64>(begin + pe->e_lfanew);

		if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
		{
			return {};
		}

		const auto optional = &ntHeader->OptionalHeader;

		const auto exportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<UCHAR*>(base) + optional->DataDirectory[
			IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		if (!MmIsAddressValid(exportDir))
		{
			return {};
		}

		const auto rvaNames = reinterpret_cast<ULONG*>(begin + exportDir->AddressOfNames);
		const auto rvaOrdinalsNames = reinterpret_cast<USHORT*>(begin + exportDir->AddressOfNameOrdinals);
		const auto rvaFunction = reinterpret_cast<ULONG*>(begin + exportDir->AddressOfFunctions);

		if (!MmIsAddressValid(rvaNames) ||
			!MmIsAddressValid(rvaOrdinalsNames) ||
			!MmIsAddressValid(rvaFunction))
		{
			return {};
		}

		for (ULONG i = 0; i < exportDir->NumberOfNames; ++i)
		{
			auto const functionName = reinterpret_cast<char*>(begin + rvaNames[i]);

			if (HASH_RUNTIME(functionName) == procHash)
			{
				return handle{ begin + rvaFunction[rvaOrdinalsNames[i]] };
			}
		}

		return {};
	}
}
