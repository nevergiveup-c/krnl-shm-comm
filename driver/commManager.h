#pragma once
#include "commThread.h"
#include "commSharedMemory.h"
#include "commCommandDispatcher.h"
#include "commHiderManager.h"

class CommManager
{
public:
	CommManager() { initialize(); }

	void initialize();
	void shutdown();

	static void handler(PVOID context) noexcept;

private:
	CommThread thread{};
	CommSharedMemory memory{};
	CommCommandDispatcher dispatcher{};
	CommHiderManager hider{};
};

#define INITIAL_COMMUNICATION static CommManager comm{};