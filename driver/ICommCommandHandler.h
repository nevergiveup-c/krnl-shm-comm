#pragma once
#include <ntdef.h>

#include "../shared/commShared.h"

class ICommCommandHandler
{
public:
	virtual ~ICommCommandHandler() = default;
	virtual NTSTATUS execute(PSHARED_DATA sharedData) const = 0;
};
