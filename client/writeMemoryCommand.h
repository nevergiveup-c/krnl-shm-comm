#pragma once
#include "ICilentCommnads.h"

class WriteMemoryCommand final : public IClientCommands
{
public:
	WriteMemoryCommand(ClientCommProtocol* prot) : IClientCommands(prot) {}
	NTSTATUS exec(DWORD targetPid, PVOID address, PVOID buffer, SIZE_T size) const;
	static UCHAR getCommandId();
};
