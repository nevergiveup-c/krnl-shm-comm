#pragma once
#include "ICommCommandHandler.h"

class ReadMemoryHandler : public ICommCommandHandler
{
public:
	NTSTATUS execute(PSHARED_DATA sharedData) const override;
	static UCHAR getCommandId();
};
