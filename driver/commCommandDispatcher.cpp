#include "commCommandDispatcher.h"

#include <ntddk.h>

#include "allocateMemoryHandler.h"
#include "freeMemoryHandler.h"
#include "readMemoryHandler.h"
#include "writeMemoryHandler.h"

CommCommandDispatcher::CommCommandDispatcher()
{
    registerCommand<ReadMemoryHandler>();
    registerCommand<WriteMemoryHandler>();
    registerCommand<AllocateMemoryHandler>();
    registerCommand<FreeMemoryHandler>();
}

void CommCommandDispatcher::dispatcher(CommSharedMemory* memory, CommThread* thread) const
{
    if (!memory->isMemoryValid())
    {
        return;
    }

    auto const context = memory->getContext();

    if (!MmIsAddressValid(context))
    {
        return;
    }

    auto const data = context->sharedData;

    if (data->commandStatus != STATUS_PROCESSING)
    {
        return;
    }

    if (data->command >= CMD_MAX)
    {
        return;
    }

    if (data->command == CMD_SHUTDOWN)
    {
        thread->setRunning(false);
        data->result = STATUS_SUCCESS;
        data->commandStatus = STATUS_COMPLETED;
        data->command = CMD_NONE;
        return;
    }

    auto const handler = handlers[data->command];

    if (!MmIsAddressValid(handler ))
    {
        return;
    }

    data->result = handler->execute(data);
    data->commandStatus = STATUS_COMPLETED;
    data->command = CMD_NONE;
}

void CommCommandDispatcher::shutdown() const
{
    for (SIZE_T i{}; i < CMD_MAX; ++i)
    {
        delete handlers[i];
    }
}
