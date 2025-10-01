#pragma once
#include "commThread.h"
#include "commSharedMemory.h"
#include "ICommCommandHandler.h"

class CommCommandDispatcher
{
public:
	CommCommandDispatcher();

	void dispatcher(CommSharedMemory* memory, CommThread* thread) const;
	void shutdown() const;

private:
	template<class Command>
	NTSTATUS registerCommand();

	template<class Command>
	void unregisterCommand();

	ICommCommandHandler* handlers[CMD_MAX]{};
};

template <class Command> NTSTATUS CommCommandDispatcher::registerCommand()
{
	auto const commandId = Command::getCommandId();
	handlers[commandId] = new Command{};
	if (handlers[commandId] == nullptr) {
		return STATUS_INVALID_ADDRESS;
	}
	return STATUS_SUCCESS;
}

template <class Command> void CommCommandDispatcher::unregisterCommand()
{
	auto const commandId = Command::getCommandId();
	delete handlers[commandId];
	handlers[commandId] = nullptr;
}
