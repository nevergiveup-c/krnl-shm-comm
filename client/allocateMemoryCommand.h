#pragma once
#include "ICilentCommnads.h"


class AllocateMemoryCommand final : public IClientCommands
{
public:
	AllocateMemoryCommand(ClientCommProtocol* prot) : IClientCommands(prot) {}
	NTSTATUS exec(DWORD targetPid, SIZE_T size, ULONG allocationType, ULONG protection, PVOID* allocatedAddress) const;
	static UCHAR getCommandId();
};
