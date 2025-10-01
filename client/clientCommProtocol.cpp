#include "clientCommProtocol.h"

#include <iostream>
#include <ntstatus.h>
#include <thread>

NTSTATUS ClientCommProtocol::send(COMMANDS cmd, DWORD timeout, bool waitResult) const
{
	if (!connection->isReady())
	{
		return STATUS_UNSUCCESSFUL;
	}

	auto const data = getData();

	data->commandStatus = STATUS_PROCESSING;
	data->command = cmd;

	if (waitResult)
	{
		DWORD startTime = GetTickCount();
		while (data->commandStatus == STATUS_PROCESSING)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (GetTickCount() - startTime > timeout)
			{
				//printf("Command timeout!\n");
				return STATUS_TIMEOUT;
			}
		}

		auto const result = data->result;
		//printf("Command completed with status: 0x%X", result);

		return result;
	}

	return STATUS_SUCCESS;
}

