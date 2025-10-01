#pragma once
#include "ICommCommandHandler.h"

class AllocateMemoryHandler : public ICommCommandHandler
{
public:
	NTSTATUS execute(PSHARED_DATA sharedData) const override;
	static UCHAR getCommandId();
};
