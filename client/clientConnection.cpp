
#include <windows.h>

#include "clientConnection.h"

bool ClientConnection::connect()
{
	section = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
		FALSE, SHARED_MEMORY_CLIENT_NAME);

	if (section == nullptr)
	{
		//logger.critical("Failed to open file mapping");
		return false;
	}

	sharedData = static_cast<PSHARED_DATA>(MapViewOfFile(section, FILE_MAP_READ | FILE_MAP_WRITE, 0,
		0, 0));

	if (sharedData == nullptr)
	{
		//logger.critical("Failed to map shared memory");
		CloseHandle(section);
		return false;
	}

	sharedData->clientStatus = STATUS_CONNECTED;

	CloseHandle(section);

	connected = true;
	return true;
}

bool ClientConnection::disconnect()
{
	if (sharedData != nullptr)
	{
		UnmapViewOfFile(sharedData);
		sharedData = nullptr;
	}

	if (section != nullptr)
	{
		CloseHandle(section);
		section = nullptr;
	}

	return true;
}

bool ClientConnection::isDriverLoaded()
{
	return OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
		FALSE, SHARED_MEMORY_CLIENT_NAME) != nullptr;
}

bool ClientConnection::isReady() const
{
	return connected;
}
