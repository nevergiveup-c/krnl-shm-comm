#include "commThreadHider.h"

#include <ntifs.h>
#include <ntddk.h>

#include "framework.h"
#include "symbols.h"
#include "utils.h"

NTSTATUS CommThreadHider::hide()
{
	if (hidden)
	{
		return STATUS_SUCCESS;
	}

	if (!isReady()) 
	{
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status{};

	status = unlinkThreadFromProcess();

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to unlink thread");
		return status;
	}

	status = clearThreadFlags();

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to clear misc flags");
		return status;
	}

	status = clearStartAddress();

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to clear start address");
		return status;
	}

	status = clearPspCidTableEntry();

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to destroy PspCidTable entry");
		return status;
	}

	hidden = true;
	return STATUS_SUCCESS;
}

NTSTATUS CommThreadHider::restore()
{
	if (!hidden) 
	{
		return STATUS_SUCCESS;
	}

	if (MmIsAddressValid(origHandleTableEntry))
	{
		*origHandleTableEntry = origCidEntry;
	}

	if (origMiscFlags.has_value())
	{
		symbols->set<LONG, &NT_OFFSETS::ktMiscFlags>(thread, origMiscFlags.get());
	}

	auto threadListEntry = symbols->get<LIST_ENTRY, &NT_OFFSETS::etThreadListEntry>(thread);
	auto threadListHead = symbols->get<LIST_ENTRY, &NT_OFFSETS::epThreadListHead>(process);

	InitializeListHead(threadListEntry);
	InsertTailList(threadListHead, threadListEntry);

	hidden = false;

	return NTSTATUS();
}

NTSTATUS CommThreadHider::unlinkThreadFromProcess() const
{
	auto data = symbols->data();
	auto threadId = PsGetThreadId(reinterpret_cast<PETHREAD>(thread));

	auto const threadListHead = symbols->get<LIST_ENTRY, &NT_OFFSETS::epThreadListHead>(process);

	for (PLIST_ENTRY list = threadListHead->Flink; list != threadListHead; list = list->Flink)
	{
		auto const entry = CONTAINING_RECORD_BY_OFFSET(list, MYTHREAD, data.etThreadListEntry);
		auto const currentThreadId = reinterpret_cast<DWORD32>(PsGetThreadId(reinterpret_cast<PETHREAD>(entry)));

		if (currentThreadId == reinterpret_cast<DWORD32>(threadId))
		{
			list->Blink->Flink = list->Flink;
			list->Flink->Blink = list->Blink;
			list->Flink = list;
			list->Blink = list;

			--(*symbols->get<UCHAR, &NT_OFFSETS::epActiveThreads>(process));

			return STATUS_SUCCESS;
		}
	}

	return STATUS_UNSUCCESSFUL;
}

NTSTATUS CommThreadHider::clearThreadFlags()
{
	auto const miscFlag = symbols->get<LONG, &NT_OFFSETS::ktMiscFlags>(thread);

	origMiscFlags.set_if_empty(*miscFlag);

	*miscFlag &= ~(1 << 3);		//Disable affinity
	*miscFlag &= ~(1 << 4);		//Disable alertable bit
	*miscFlag &= ~(1 << 10);	//Disable system thread bit
	*miscFlag &= ~(1 << 14);	//Disable APC queueable bit

	if (PsIsSystemThread(reinterpret_cast<PETHREAD>(thread)))
	{
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

NTSTATUS CommThreadHider::clearPspCidTableEntry()
{
	PETHREAD tempThread{ nullptr };
	auto threadId = PsGetThreadId(reinterpret_cast<PETHREAD>(thread));

	if (!NT_SUCCESS(PsLookupThreadByThreadId(threadId, &tempThread)))
	{
		return STATUS_SUCCESS;
	}

	removePspCidTableHandle(symbols->get<MYCLIENT_ID, &NT_OFFSETS::etCid>(thread)->UniqueThread);

	if (NT_SUCCESS(PsLookupThreadByThreadId(threadId, &tempThread)))
	{
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

NTSTATUS CommThreadHider::clearStartAddress()
{
	auto const etwpLogger = symbols->get<void, &NT_OFFSETS::fEtwpLogger>(getNtoskrnlBase());

	if (etwpLogger == nullptr)
	{
		return STATUS_UNSUCCESSFUL;
	}

	symbols->set<PVOID, &NT_OFFSETS::etStartAddress>(thread, etwpLogger);
	symbols->set<PVOID, &NT_OFFSETS::etWin32StartAddress>(thread, etwpLogger);

	return STATUS_SUCCESS;
}

NTSTATUS CommThreadHider::removePspCidTableHandle(HANDLE threadId)
{
	auto const handleTableAddress = *symbols->get<PVOID, &NT_OFFSETS::gPspCidTable>(getNtoskrnlBase());

	if (!MmIsAddressValid(handleTableAddress))
	{
		return STATUS_UNSUCCESSFUL;
	}

	origHandleTableEntry = reinterpret_cast<PULONG64>(
		expLookupHandleTableEntry(
			static_cast<unsigned int*>(handleTableAddress),
			reinterpret_cast<__int64>(threadId)
		)
		);

	if (!MmIsAddressValid(origHandleTableEntry))
	{
		return STATUS_UNSUCCESSFUL;
	}

	origCidEntry = *origHandleTableEntry;
	*origHandleTableEntry = NULL;

	return STATUS_SUCCESS;
}
