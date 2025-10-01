#pragma once
#include "commSharedMemory.h"

class CommHiderBase
{
public:
	explicit CommHiderBase(CommSharedMemory* mem) : memory(mem)
	{
		initialized = initialize();
	}

	virtual ~CommHiderBase() = default;

	virtual NTSTATUS hide() = 0;
	virtual NTSTATUS restore() = 0;
	virtual bool isHidden() const = 0;

protected:
	PVOID getNtoskrnlBase() const { return ntoskrnlBase; }
	CommSharedMemory* getMemory() const { return memory; }
	virtual bool isReady() const { return initialized; }

private:
	bool initialize();
	bool initialized{ false };
	CommSharedMemory* memory{ nullptr };
	PVOID ntoskrnlBase{ nullptr };
};
