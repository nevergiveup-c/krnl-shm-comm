#include "utils.h"

#include <ntddk.h>
#include <wdm.h>
#include <ntimage.h>

#include "framework.h"

#include "ntenums.h"
#include "ntclasses.h"
#include "ntdecl.h"

long long expLookupHandleTableEntry(unsigned int* a1, long long a2)
{
    unsigned __int64 v2; // rdx
    __int64 v3; // r8
    __int64 v4; // rax

    v2 = a2 & 0xFFFFFFFFFFFFFFFCuLL;
    if (v2 >= *a1)
        return 0LL;
    v3 = *((LONG64*)a1 + 1);
    if ((v3 & 3) == 1)
    {
        v4 = *(LONG64*)(v3 + 8 * (v2 >> 10) - 1);
        return v4 + 4 * (v2 & 0x3FF);
    }
    if ((v3 & 3) != 0)
    {
        v4 = *(LONG64*)(*(LONG64*)(v3 + 8 * (v2 >> 19) - 2) + 8 * ((v2 >> 10) & 0x1FF));
        return v4 + 4 * (v2 & 0x3FF);
    }
    return v3 + 4 * v2;
}

PVOID getBaseAddress(const char* driver)
{
    ULONG bufferSize {};
    PRTL_PROCESS_MODULES pModules{};

    NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation,
        nullptr, 0, &bufferSize);

    if (status != STATUS_INFO_LENGTH_MISMATCH)
    {
        return nullptr;
    }

    pModules = static_cast<PRTL_PROCESS_MODULES>(ExAllocatePoolWithTag(NonPagedPool, bufferSize, 'dgba'));

    if (!pModules)
    {
        return nullptr;
    }

    status = ZwQuerySystemInformation(SystemModuleInformation, pModules, bufferSize, &bufferSize);

    if (!NT_SUCCESS(status))
    {
        ExFreePoolWithTag(pModules, 'dgba');
        return nullptr;
    }

    for (ULONG i = 0; i < pModules->NumberOfModules; i++)
    {
        PCHAR fileName = reinterpret_cast<PCHAR>(pModules->Modules[i].FullPathName) + pModules->Modules[i].OffsetToFileName;

        if (_stricmp(fileName, driver) == 0)
        {
            PVOID base = pModules->Modules[i].ImageBase;
            ExFreePoolWithTag(pModules, 'dgba');
            return base;
        }
    }

    ExFreePoolWithTag(pModules, 'dgba');
    return nullptr;
}

SectionData getSection(ULONG64 base, const char* name)
{
    const auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return {};
    }

    auto ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
    {
        return {};
    }

    const auto sections = reinterpret_cast<IMAGE_SECTION_HEADER*>(reinterpret_cast<uintptr_t>(ntHeaders) + sizeof(IMAGE_NT_HEADERS64));

    for (ULONG i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
    {
        // Limit yourself to only executable non-discardable sections
        if (!(sections[i].Characteristics & IMAGE_SCN_CNT_CODE) ||
            !(sections[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) ||
            sections[i].Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
        {
            continue;
        }
        if (strncmp(reinterpret_cast<const char*>(sections[i].Name), name, IMAGE_SIZEOF_SHORT_NAME) == 0)
        {
            auto const test = sections[i];


            const uintptr_t sectionStart = base + sections[i].VirtualAddress;
            const ULONG sectionSize = sections[i].Misc.VirtualSize;

            return { .mBegin = sectionStart, .mSize = sectionSize };
        }
    }

    return {};
}

ProcessHandle::ProcessHandle(ULONG processId)
{
	CLIENT_ID clientId{};
	OBJECT_ATTRIBUTES objAttrs;

	InitializeObjectAttributes(&objAttrs, nullptr, 0, nullptr, nullptr)
		clientId.UniqueProcess = reinterpret_cast<HANDLE>(processId);

	NTSTATUS status = ZwOpenProcess(&handle, 0x0008, &objAttrs, &clientId);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to open process %d: 0x%X", processId, status);
		handle = nullptr;
	}
}

ProcessHandle::~ProcessHandle()
{
	if (handle)
	{
		ZwClose(handle);
	}
}
