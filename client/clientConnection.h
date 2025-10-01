#pragma once
#include "../shared/commShared.h"

class ClientConnection
{
public:
	bool connect();
	bool disconnect();

	static bool isDriverLoaded();

	PSHARED_DATA getData() const { return sharedData; }
	bool isReady() const;

private:
	HANDLE section{ nullptr };
	PSHARED_DATA sharedData{ nullptr };
	bool connected{ false };
};
