#include "writeMemoryHandler.h"

#include <ntdef.h>
#include <ntifs.h>
#include <wdm.h>

#include "framework.h"
#include "ntdecl.h"

NTSTATUS WriteMemoryHandler::execute(PSHARED_DATA sharedData) const
{
	auto const& mem = sharedData->memory;

	PEPROCESS targetProcess;

	NTSTATUS status = PsLookupProcessByProcessId(ULongToHandle(sharedData->processId), &targetProcess);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	PEPROCESS sourceProcess{ PsGetCurrentProcess() };

	SIZE_T bytesWritten = 0;
	status = MmCopyVirtualMemory(
		sourceProcess,
		sharedData->dataBuffer,
		targetProcess,
		mem.targetAddress,
		mem.size,
		KernelMode,
		&bytesWritten
	);

	ObDereferenceObject(targetProcess);

	LOG("Write memory: addr=0x%p, size=0x%zx, written=0x%zx, status=0x%X",
		mem.targetAddress, mem.size, bytesWritten, status);

	return status;
}

UCHAR WriteMemoryHandler::getCommandId()
{
	return COMMANDS::CMD_WRITE_MEMORY;
}
