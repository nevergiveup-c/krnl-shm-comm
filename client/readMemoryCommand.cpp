#include "readMemoryCommand.h"

#include <ntstatus.h>

NTSTATUS ReadMemoryCommand::exec(DWORD targetPid, PVOID address, PVOID buffer, SIZE_T size) const
{
	const auto data = protocol->getData();

	data->processId = targetPid;
	data->memory.targetAddress = address;
	data->memory.size = size;

	auto const status = protocol->send(CMD_READ_MEMORY);

	if (status == STATUS_SUCCESS)
	{
		memcpy(buffer, data->dataBuffer, size);
		return STATUS_SUCCESS;
	}

	return status;
}

UCHAR ReadMemoryCommand::getCommandId()
{
	return static_cast<int>(COMMANDS::CMD_READ_MEMORY);
}
