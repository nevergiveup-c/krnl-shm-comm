#include "allocateMemoryHandler.h"

#include <ntifs.h>

#include "commSharedMemory.h"
#include "framework.h"
#include "utils.h"

NTSTATUS AllocateMemoryHandler::execute(PSHARED_DATA sharedData) const
{
	auto const& alloc = sharedData->allocation;

	ProcessHandle handle(sharedData->processId);

	if (!handle.isValid())
	{
		return STATUS_INVALID_HANDLE;
	}

	PVOID baseAddress = nullptr;
	SIZE_T regionSize = alloc.size;

	NTSTATUS status = ZwAllocateVirtualMemory(
		handle.get(),
		&baseAddress,
		0,
		&regionSize,
		alloc.allocationType,
		alloc.protection
	);

	if (NT_SUCCESS(status))
	{
		sharedData->allocation.allocatedAddress = baseAddress;
	}

	LOG("Allocate memory: size=0x%zx, addr=0x%p, status=0x%X",
		alloc.size, baseAddress, status);

	return status;
}

UCHAR AllocateMemoryHandler::getCommandId()
{
	return COMMANDS::CMD_ALLOCATE_MEMORY;
}
