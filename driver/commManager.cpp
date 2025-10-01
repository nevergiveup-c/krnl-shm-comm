#include <ntifs.h>

#include "commManager.h"

#include <intrin.h>

#include "framework.h"
#include "patterns.h"

namespace
{
	void delay(const ULONG ms)
	{
		LARGE_INTEGER delay;
		delay.QuadPart = -10000LL * ms;
		KeDelayExecutionThread(KernelMode, FALSE, &delay);
	}
}

void CommManager::initialize()
{
	if (!NT_SUCCESS(thread.createThread(handler, this)))
	{
		LOG("Failed to create system thread");
	}
}

void CommManager::shutdown()
{
	if (memory.isMemoryValid())
	{
		hider.shutdown();
		dispatcher.shutdown();
		memory.unmapMemory();
		thread.terminateThread();
	}
}

void CommManager::handler(PVOID context) noexcept
{
	const auto mgr = static_cast<CommManager*>(context);

	mgr->hider.initialize(&mgr->memory);

	if (!NT_SUCCESS(mgr->memory.mapMemory()))
	{
		return;
	}

	LOG("Memory initialized: %p", mgr->memory.getContext()->kernelVirtualAddress);

	if (!NT_SUCCESS(mgr->hider.getThreadHider()->hide()))
	{
		return;
	}

	while (mgr->memory.getContext()->sharedData->
		clientStatus == STATUS_CONNECTION)
	{
		delay(100);
	}

	if (!NT_SUCCESS(mgr->hider.getSharedMemoryHider()->hide()))
	{
		return;
	}

	LOG("Driver ready for communication");

	while (mgr->thread.isRunning())
	{
		mgr->dispatcher.dispatcher(&mgr->memory, &mgr->thread);
		delay(1000);
	}

	LOG("Dispatcher thread shutdown");
	mgr->shutdown();
}
