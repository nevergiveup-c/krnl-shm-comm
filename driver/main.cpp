// ReSharper disable CppClangTidyMiscUseInternalLinkage
#include <ntifs.h>
#include <wdm.h>

#include "commManager.h"
#include "framework.h"
#include "symbols.h"

NTSTATUS CustomDriverEntry(
	_In_ const HANDLE pid,
	_In_ const PVOID data)
{
	LOAD_NT_OFFSETS(pid, data, sizeof(NT_OFFSETS))

	LOG("Mapped driver loaded");

	INITIAL_COMMUNICATION

	LOG("Driver loaded successfully");

	return STATUS_SUCCESS;
}
