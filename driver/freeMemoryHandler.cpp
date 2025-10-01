#include "freeMemoryHandler.h"

#include <ntifs.h>

#include "commSharedMemory.h"
#include "framework.h"
#include "utils.h"

NTSTATUS FreeMemoryHandler::execute(PSHARED_DATA sharedData) const
{
	auto const handle = ProcessHandle(sharedData->processId);

	if (!handle.isValid())
	{
		return STATUS_INVALID_HANDLE;
	}

	auto const free = sharedData->free;

	PVOID base = free.address;
	SIZE_T regionSize = free.size;

	auto const status = ZwFreeVirtualMemory(
		handle.get(),
		&base,
		&regionSize,
		free.freeType
	);

	LOG("Free memory: addr=0x%p, size=0x%zx, status=0x%X",
		free.address, free.size, status);

	return status;
}

UCHAR FreeMemoryHandler::getCommandId()
{
	return COMMANDS::CMD_FREE_MEMORY;
}
