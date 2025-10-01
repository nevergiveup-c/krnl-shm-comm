#pragma once
#include <array>
#include <memory>
#include <utility>

#include "clientCommProtocol.h"

#include "ICilentCommnads.h"

#include "readMemoryCommand.h"
#include "writeMemoryCommand.h"
#include "allocateMemoryCommand.h"
#include "freeMemoryCommand.h"

class ClientCommandManager
{
public:
	ClientCommandManager(ClientCommProtocol* prot) : protocol(prot)
	{
		registerCommand<ReadMemoryCommand>(prot);
		registerCommand<WriteMemoryCommand>(prot);
		registerCommand<AllocateMemoryCommand>(prot);
		registerCommand<FreeMemoryCommand>(prot);
	}

	template<class Command, class ...Args>
	NTSTATUS execute(Args&&... args);

private:

	template<class Command>
	NTSTATUS registerCommand(ClientCommProtocol* prot);

	template<class Command>
	void unregisterCommand();

	ClientCommProtocol* protocol{ nullptr };
	std::array<std::unique_ptr<IClientCommands>, static_cast<size_t>(COMMANDS::CMD_MAX)> commands{};
};

template<class Command, class ...Args>
NTSTATUS ClientCommandManager::execute(Args&&... args)
{
	auto const index = Command::getCommandId();

	if (index >= static_cast<int>(COMMANDS::CMD_MAX) || !commands[index])
	{
		return STATUS_INVALID_PARAMETER;
	}

	auto* cmd = static_cast<Command*>(commands[index].get());
	return cmd->exec(std::forward<Args>(args)...);
}

template <class Command> NTSTATUS ClientCommandManager::registerCommand(ClientCommProtocol* prot)
{
	auto const commandId = Command::getCommandId();
	commands[commandId] = std::make_unique<Command>(prot);
	if (commands[commandId] == nullptr) {
		return STATUS_INVALID_ADDRESS;
	}
	return STATUS_SUCCESS;
}

template <class Command> void ClientCommandManager::unregisterCommand()
{
	auto const commandId = Command::getCommandId();
	delete commands[commandId];
	commands[commandId] = nullptr;
}
