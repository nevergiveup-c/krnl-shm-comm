#pragma once
#include "commHiderBase.h"
#include "ntclasses.h"
#include "optional.h"

class CommThreadHider final : public CommHiderBase
{
public:
	explicit CommThreadHider(CommSharedMemory* mem, PMYTHREAD currentThread, PMYPROCESS currentProcess) :
		CommHiderBase(mem), thread(currentThread), process(currentProcess) {}

	NTSTATUS hide() override;
	NTSTATUS restore() override;
	bool isHidden() const override { return hidden; }

private:

	NTSTATUS unlinkThreadFromProcess() const;
	NTSTATUS clearThreadFlags();
	NTSTATUS clearPspCidTableEntry();
	NTSTATUS clearStartAddress();

	NTSTATUS removePspCidTableHandle(HANDLE threadId);

	PMYTHREAD thread{ nullptr };
	PMYPROCESS process{ nullptr };
	bool hidden{ false };

	optional<LONG> origMiscFlags{};
	PULONG64 origHandleTableEntry{};
	ULONG64 origCidEntry{};
};

