#pragma once
#include <windows.h>

#include "clientConnection.h"

class ClientCommProtocol
{
public:
	ClientCommProtocol(ClientConnection* conn) : connection(conn) {}
	NTSTATUS send(COMMANDS cmd, DWORD timeout = 5000, bool waitResult = true) const;

	PSHARED_DATA getData() const { return connection->getData(); }

private:
	ClientConnection* connection{ nullptr };
};
