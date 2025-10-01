#include "readMemoryHandler.h"

#include <ntifs.h>

#include "framework.h"
#include "ntdecl.h"

NTSTATUS ReadMemoryHandler::execute(PSHARED_DATA sharedData) const
{
	auto const shared = sharedData;
	auto const& mem = shared->memory;

	PEPROCESS sourceProcess;

	NTSTATUS status = PsLookupProcessByProcessId(ULongToHandle(shared->processId), &sourceProcess);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	PEPROCESS targetProcess{ PsGetCurrentProcess() };

	SIZE_T bytesRead = 0;
	status = MmCopyVirtualMemory(
		sourceProcess,
		mem.targetAddress,
		targetProcess,
		sharedData->dataBuffer,
		mem.size,
		KernelMode,
		&bytesRead
	);

	ObDereferenceObject(sourceProcess);

	LOG("Read memory: addr=0x%p, size=0x%zx, read=0x%zx, status=0x%X",
		mem.targetAddress, mem.size, bytesRead, status);

	return status;
}

UCHAR ReadMemoryHandler::getCommandId()
{
	return COMMANDS::CMD_READ_MEMORY;
}
