#pragma once
#include "ICilentCommnads.h"

class ReadMemoryCommand final : public IClientCommands
{
public:
	ReadMemoryCommand(ClientCommProtocol* prot) : IClientCommands(prot) {}
	NTSTATUS exec(DWORD targetPid, PVOID address, PVOID buffer, SIZE_T size) const;
	static UCHAR getCommandId();
};
