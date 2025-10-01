#pragma once
#include <ntdef.h>

#include "../shared/commShared.h"

class CommSharedMemory
{
public:
	NTSTATUS mapMemory();
	NTSTATUS unmapMemory();
	PSHARED_CONTEXT getContext();
	bool isMemoryValid() const;

private:
	NTSTATUS initSecDescriptor();

	SHARED_CONTEXT sharedContext
	{
		.sectionHandle = nullptr,
		.kernelVirtualAddress = nullptr,
		.sharedData = nullptr
	};

	PVOID dacl{};
	UCHAR secDesc[40]{}; //SECURITY_DESCRIPTOR 
};
