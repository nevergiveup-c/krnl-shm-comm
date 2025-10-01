#include "freeMemoryCommand.h"

NTSTATUS FreeMemoryCommand::exec(DWORD targetPid, PVOID address, SIZE_T size, ULONG freeType) const
{
	const auto data = protocol->getData();

	data->processId = targetPid;
	data->free.address = address;
	data->free.size = size;
	data->free.freeType = freeType;

	return protocol->send(CMD_FREE_MEMORY);
}

UCHAR FreeMemoryCommand::getCommandId()
{
	return static_cast<int>(COMMANDS::CMD_FREE_MEMORY);
}
