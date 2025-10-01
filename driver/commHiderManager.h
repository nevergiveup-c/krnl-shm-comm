// ReSharper disable CppClangTidyCppcoreguidelinesProTypeStaticCastDowncast
// ReSharper disable CppClangTidyPerformanceEnumSize
// ReSharper disable CppClangTidyReadabilityUseAnyofallof

#pragma once
#include "commShreadMemoryHider.h"
#include "commThreadHider.h"

class CommHiderManager
{
	enum HIDER_INDEX
	{
		THREAD_HIDER = 0,
		SHARED_MEM_HIDER,
		HIDER_MAX,
	};
public:
	void initialize(CommSharedMemory* mem);
	void shutdown();

	CommThreadHider* getThreadHider() const{ return static_cast<CommThreadHider*>(hider[THREAD_HIDER]); }
	CommSharedMemoryHider* getSharedMemoryHider() const { return static_cast<CommSharedMemoryHider*>(hider[SHARED_MEM_HIDER]); }

	NTSTATUS hideAll();
	NTSTATUS restoreAll();
	bool isHidden() const;

private:
	CommHiderBase* hider[HIDER_MAX]{};
};

inline void CommHiderManager::initialize(CommSharedMemory* mem)
{
	hider[THREAD_HIDER] = new CommThreadHider
	{
		mem,
		reinterpret_cast<PMYTHREAD>(PsGetCurrentThread()),
		reinterpret_cast<PMYPROCESS>(PsGetCurrentProcess())
	};

	hider[SHARED_MEM_HIDER] = new CommSharedMemoryHider{ mem };
}

inline void CommHiderManager::shutdown()
{
	if (auto const status = restoreAll(); !NT_SUCCESS(status))
	{
		//TODO: LOGG
	}

	for (auto& h : hider)
	{
		if (!MmIsAddressValid(h))
		{
			continue;
		}

		delete h;
		h = nullptr;
	}
}

inline NTSTATUS CommHiderManager::hideAll()
{
	for (const auto h : hider)
	{
		if (!MmIsAddressValid(h) || h->isHidden())
		{
			continue;
		}
		if (auto const status = h->hide(); !NT_SUCCESS(status))
		{
			return status;
		}
	}

	return STATUS_SUCCESS;
}

inline NTSTATUS CommHiderManager::restoreAll()
{
	for (const auto h : hider)
	{
		if (!MmIsAddressValid(h) || !h->isHidden())
		{
			continue;
		}

		if (auto const status = h->restore(); !NT_SUCCESS(status))
		{
			//return commandStatus;
		}
	}

	return STATUS_SUCCESS;
}

inline bool CommHiderManager::isHidden() const
{
	for (const auto h : hider)
	{
		if (MmIsAddressValid(h) && h->isHidden())
		{
			return true;
		}
	}
	return false;
}
