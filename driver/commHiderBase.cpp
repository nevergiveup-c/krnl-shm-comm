
#include "commHiderBase.h"

#include <ntddk.h>
#include <wdm.h>

#include "framework.h"
#include "utils.h"

bool CommHiderBase::initialize()
{
	ntoskrnlBase = getBaseAddress("ntoskrnl.exe");

	if (!MmIsAddressValid(ntoskrnlBase))
	{
		LOG("Failed to initialize hider base");
		return false;
	}

	return true;
}
