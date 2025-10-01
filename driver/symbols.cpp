#include "symbols.h"

#include <ntifs.h>
#include <wdm.h>

bool Symbols::initialize(const HANDLE pid, const PVOID data, const SIZE_T size)
{
	KAPC_STATE apc;
	PEPROCESS process{};

	if (auto const status = PsLookupProcessByProcessId(pid, &process); !NT_SUCCESS(status))
	{
		return false;
	}

	KeStackAttachProcess(process, &apc);
	memcpy(&offsets, data, size);
	KeUnstackDetachProcess(&apc);

	ObDereferenceObject(process);

	return true;
}
