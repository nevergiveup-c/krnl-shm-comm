#pragma once
#include <ntddk.h>
#include <ntdef.h>

class CommThread
{
public:
	NTSTATUS createThread(PKSTART_ROUTINE routine, PVOID context);
	void terminateThread() const;

	[[nodiscard]] bool isRunning() const { return running; }
	void setRunning(const bool state) { running = state; }

private:
	volatile bool running{ true };
};
