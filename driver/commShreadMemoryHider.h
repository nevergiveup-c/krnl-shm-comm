#pragma once

#include "commHiderBase.h"

#include "ntclasses.h"

class CommSharedMemoryHider final : public CommHiderBase
{
public:
    explicit CommSharedMemoryHider(CommSharedMemory* mem)
        : CommHiderBase(mem) {}

    NTSTATUS hide() override;
    NTSTATUS restore() override;
    bool isHidden() const override { return hidden; }

private:
    NTSTATUS spoofObjectHider();
    NTSTATUS removeObjectFromDirectory();
    NTSTATUS removeHandleFromObject();
    NTSTATUS removeVad();

	_OBJECT_DIRECTORY* findDirectory(const WCHAR* dirName);
    _MMVAD* findVad(const _RTL_AVL_TREE* tree, ULONG64 vpn);

    bool hidden{ false };

    _MMVAD* origVad{ nullptr };
    _RTL_BALANCED_NODE* parentVad{ nullptr };
    BOOLEAN wasRightChild{ false };

    ULONG bucketIdx{ 0 };
    _OBJECT_DIRECTORY* baseNamedObjectDir{ nullptr };
    _OBJECT_DIRECTORY_ENTRY* origObjectEntry{ nullptr };

    _OBJECT_HEADER* objectHeader{ nullptr };
    _OBJECT_HEADER origObjectHeader{};
    PULONG64 origHandleTableEntry{ nullptr };
    ULONG64 origHandleEntry{};
};
