#include "commThread.h"
#include "framework.h"

NTSTATUS CommThread::createThread(const PKSTART_ROUTINE routine, const PVOID context)
{
	HANDLE dummyHandle{};

	NTSTATUS status = PsCreateSystemThread(&dummyHandle,
		THREAD_ALL_ACCESS,
		nullptr,
		nullptr,
		nullptr,
		routine,
		context);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to get thread object: 0x%X", status);
		running = false;
		return status;
	}

	ZwClose(dummyHandle);
	dummyHandle = nullptr;

	LOG("System thread created successfully");
	return STATUS_SUCCESS;
}

void CommThread::terminateThread() const
{
	PsTerminateSystemThread(STATUS_SUCCESS);
}
