#include "allocateMemoryCommand.h"

#include <ntstatus.h>

NTSTATUS AllocateMemoryCommand::exec(DWORD targetPid, SIZE_T size, ULONG allocationType, ULONG protection, PVOID* allocatedAddress) const
{
	const auto data = protocol->getData();

	data->processId = targetPid;
	data->allocation.size = size;
	data->allocation.allocationType = allocationType;
	data->allocation.protection = protection;
	data->allocation.allocatedAddress = nullptr;

	auto const status = protocol->send(CMD_ALLOCATE_MEMORY);

	if (status == STATUS_SUCCESS)
	{
		*allocatedAddress = data->allocation.allocatedAddress;
		return STATUS_SUCCESS;
	}

	return status;
}

UCHAR AllocateMemoryCommand::getCommandId()
{
	return static_cast<int>(COMMANDS::CMD_ALLOCATE_MEMORY);
}
