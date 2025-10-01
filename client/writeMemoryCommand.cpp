#include "writeMemoryCommand.h"

NTSTATUS WriteMemoryCommand::exec(DWORD targetPid, PVOID address, PVOID buffer, SIZE_T size) const
{
	const auto data = protocol->getData();

	if (size > sizeof(data->dataBuffer))
	{
		return false;
	}

	memcpy(data->dataBuffer, buffer, size);

	data->processId = targetPid;
	data->memory.targetAddress = address;
	data->memory.size = size;

	return protocol->send(CMD_WRITE_MEMORY);
}

UCHAR WriteMemoryCommand::getCommandId()
{
	return static_cast<int>(COMMANDS::CMD_WRITE_MEMORY);
}
