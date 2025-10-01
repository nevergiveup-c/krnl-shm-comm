#pragma once
#include <memory>
#include <ntstatus.h>

#include "clientCommands.h"
#include "clientCommProtocol.h"

class DriverClient
{
public:
	DriverClient()
	{
		connection = std::make_unique<ClientConnection>();
		protocol = std::make_unique<ClientCommProtocol>(connection.get());
		manager = std::make_unique<ClientCommandManager>(protocol.get());
	}

	bool connect() const { return connection->connect(); }
	bool disconnect() const
	{
		auto const status = protocol->send(CMD_SHUTDOWN);
		return status == STATUS_SUCCESS && connection->disconnect();
	}

	template<class Command, class ...Args>
	bool command(Args&&... args) {
		return manager->execute<Command>(std::forward<Args>(args)...);
	}

private:
	std::unique_ptr<ClientConnection> connection; 
	std::unique_ptr<ClientCommProtocol> protocol;
	std::unique_ptr<ClientCommandManager> manager;
};
