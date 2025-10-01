#pragma once
#include "ICilentCommnads.h"

class FreeMemoryCommand final : public IClientCommands
{
public:
	FreeMemoryCommand(ClientCommProtocol* prot) : IClientCommands(prot) {}
	NTSTATUS exec(DWORD targetPid, PVOID address, SIZE_T size, ULONG freeType) const;
	static UCHAR getCommandId();
};
