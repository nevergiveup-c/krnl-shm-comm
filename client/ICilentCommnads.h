#pragma once
#include "clientCommProtocol.h"

class IClientCommands
{
public:
	IClientCommands(ClientCommProtocol* prot) : protocol(prot) {}
	virtual ~IClientCommands() = default;

protected:
	ClientCommProtocol* protocol;
};