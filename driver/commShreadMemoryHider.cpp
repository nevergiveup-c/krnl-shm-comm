
#include "commShreadMemoryHider.h"

#include <cstddef>
#include <ntddk.h>

#include "framework.h"
#include "ntclasses.h"
#include "ntdecl.h"
#include "symbols.h"
#include "utils.h"

NTSTATUS CommSharedMemoryHider::hide()
{
	NTSTATUS status{};

    status = removeObjectFromDirectory();

    if (!NT_SUCCESS(status))
    {
        LOG("Failed to unlink object");
        return status;
    }

	status = removeHandleFromObject();

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to unlink handle");
		return status;
	}

    status = removeVad();

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to unlink vad");
		return status;
	}

	hidden = true;

	return STATUS_SUCCESS;
}

NTSTATUS CommSharedMemoryHider::restore()
{
    auto const process = PsGetCurrentProcess();
    auto const sectionObjectType = *symbols->get<_OBJECT_TYPE*, &NT_OFFSETS::gMmSectionObjectType>(getNtoskrnlBase());

    ++sectionObjectType->TotalNumberOfObjects;

    {
        auto& bucket = baseNamedObjectDir->HashBuckets[bucketIdx];

        if (bucket == nullptr)
        {
            bucket = origObjectEntry;
            origObjectEntry->ChainLink = nullptr;
        }
        else
        {
            origObjectEntry->ChainLink = bucket;
            bucket = origObjectEntry;
        }
    }

    ++sectionObjectType->TotalNumberOfHandles;

    {
        if (MmIsAddressValid(origHandleTableEntry))
        {
            *origHandleTableEntry = origHandleEntry;
        }
    }

    {
        auto const tree = symbols->get<_RTL_AVL_TREE, &NT_OFFSETS::epVadRoot>(process);

        origVad->Core.VadNode.Left = nullptr;
        origVad->Core.VadNode.Right = nullptr;
        origVad->Core.VadNode.ParentValue = 0;

        RtlAvlInsertNodeEx(tree, parentVad, wasRightChild, &origVad->Core.VadNode);
        ++*symbols->get<UCHAR, &NT_OFFSETS::epVadCount>(process);
    }

    hidden = false;
    return STATUS_SUCCESS;
}

_OBJECT_DIRECTORY* CommSharedMemoryHider::findDirectory(const WCHAR* dirName)
{
    auto const rootDir = *symbols->get<_OBJECT_DIRECTORY*, &NT_OFFSETS::gObpRootDirectoryObject>(getNtoskrnlBase());
    auto const infoMaskToOffset = symbols->get<UCHAR, &NT_OFFSETS::gObpInfoMaskToOffset>(getNtoskrnlBase());

    for (auto const entry : rootDir->HashBuckets)
    {
        auto chainLink = entry;

        while (MmIsAddressValid(chainLink))
        {
            auto header = OBJECT_TO_OBJECT_HEADER(chainLink->Object)

            if (!MmIsAddressValid(header) || (header->InfoMask & 0x02) == 0)
            {
                chainLink = chainLink->ChainLink;
                continue;
            }

            auto const nameInfoOffset = infoMaskToOffset[header->InfoMask & 0x03];

            auto const nameInfo = reinterpret_cast<_OBJECT_HEADER_NAME_INFO*>(
                reinterpret_cast<char*>(header) - nameInfoOffset);

            if (nameInfo->Name.Length == 0 || !MmIsAddressValid(nameInfo->Name.Buffer))
            {
                chainLink = chainLink->ChainLink;
                continue;
            }

            if (wcscmp(nameInfo->Name.Buffer, dirName) == 0)
            {
                return reinterpret_cast<_OBJECT_DIRECTORY*>(header);
            }

            chainLink = chainLink->ChainLink;
        }
    }

    return nullptr;
}

_MMVAD* CommSharedMemoryHider::findVad(const _RTL_AVL_TREE* tree, const ULONG64 vpn)
{
    auto current = tree->Root;

    while (MmIsAddressValid(current))
    {
        auto const vad = CONTAINING_RECORD(current, _MMVAD_SHORT, VadNode);

        if (vpn >= vad->StartingVpn && vpn <= vad->EndingVpn) 
        {
            return reinterpret_cast<_MMVAD*>(vad);
        }

        if (vpn < vad->StartingVpn) 
        {
            current = current->Left;
        }
        else 
        {
            current = current->Right;
        }
    }

    return nullptr;
}

NTSTATUS CommSharedMemoryHider::spoofObjectHider()
{
    auto const ctx = getMemory()->getContext();

    PVOID object{};

    auto const sectionObjectType = *symbols->get<_OBJECT_TYPE*, &NT_OFFSETS::gMmSectionObjectType>(getNtoskrnlBase());
    auto const symbolLinkType = *symbols->get<_OBJECT_TYPE*, &NT_OFFSETS::gObpSymbolicLinkObjectType>(getNtoskrnlBase());
    auto const headerCookie = *symbols->get<ULONG, &NT_OFFSETS::gObHeaderCookie>(getNtoskrnlBase());

    auto const status = ObReferenceObjectByHandle(
        ctx->sectionHandle,
        NULL,
        sectionObjectType,
        KernelMode,
		&object,
        nullptr
    );

    if (!MmIsAddressValid(object) || !NT_SUCCESS(status))
    {
        return STATUS_UNSUCCESSFUL;
    }

	objectHeader = OBJECT_TO_OBJECT_HEADER(object);

    ObDereferenceObject(object);

    if (!MmIsAddressValid(objectHeader))
    {
        return STATUS_UNSUCCESSFUL;
    }

    memcpy(&origObjectHeader, objectHeader, sizeof(_OBJECT_HEADER));

    objectHeader->PermanentObject = 1;
    objectHeader->TypeIndex = headerCookie ^ symbolLinkType->Index ^ ((reinterpret_cast<ULONG64>(objectHeader) >> 8) & 0xFF);
    objectHeader->HandleCount = 0;
    objectHeader->NextToFree = nullptr;

    auto const symbolicLink = OBJECT_HEADER_TO_OBJECT(objectHeader, _OBJECT_SYMBOLIC_LINK)

    RtlInitUnicodeString(&symbolicLink->LinkTarget, L"\\BaseNamedObject");

    return STATUS_SUCCESS;
}

NTSTATUS CommSharedMemoryHider::removeObjectFromDirectory()
{
    auto const ctx = getMemory()->getContext();
    auto const sectionObjectType = *symbols->get<_OBJECT_TYPE*, &NT_OFFSETS::gMmSectionObjectType>(getNtoskrnlBase());

    PVOID object{};

    auto const status = ObReferenceObjectByHandle(
        ctx->sectionHandle,
        NULL,
        sectionObjectType,
        KernelMode,
        &object,
        nullptr
    );

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    baseNamedObjectDir = findDirectory(L"BaseNamedObjects");

    if (!MmIsAddressValid(baseNamedObjectDir))
    {
        ObDereferenceObject(object);
        return STATUS_UNSUCCESSFUL;
    }

    for (auto& entry : baseNamedObjectDir->HashBuckets)
    {
        _OBJECT_DIRECTORY_ENTRY* prevLink{ nullptr };
        auto chainLink = entry;

        while (MmIsAddressValid(chainLink))
        {
            if (chainLink->Object == object)
            {
                origObjectEntry = chainLink;

                if (prevLink == nullptr)
                {
                    entry = chainLink->ChainLink;
                }
                else
                {
                    prevLink->ChainLink = chainLink->ChainLink;
                }

                --sectionObjectType->TotalNumberOfObjects;

                ObDereferenceObject(object);
                return STATUS_SUCCESS;
            }

            prevLink = chainLink;
            chainLink = chainLink->ChainLink;
        }

        bucketIdx++;
    }

    ObDereferenceObject(object);
    return STATUS_NOT_FOUND;
}

NTSTATUS CommSharedMemoryHider::removeHandleFromObject()
{
    auto const ctx = getMemory()->getContext();
    auto const handle = reinterpret_cast<ULONG64>(ctx->sectionHandle) ^ 0xFFFFFFFF80000000uLL;
    auto const krnlHandleTable = *symbols->get<_HANDLE_TABLE*, &NT_OFFSETS::gObpKernelHandleTable>(getNtoskrnlBase());
    auto const sectionObjectType = *symbols->get<_OBJECT_TYPE*, &NT_OFFSETS::gMmSectionObjectType>(getNtoskrnlBase());

    if (!MmIsAddressValid(krnlHandleTable))
    {
        return STATUS_UNSUCCESSFUL;
    }

    origHandleTableEntry = reinterpret_cast<PULONG64>(
        expLookupHandleTableEntry(
            reinterpret_cast<unsigned int*>(krnlHandleTable),
            handle
        )
        );

    if (!MmIsAddressValid(origHandleTableEntry))
    {
        return STATUS_UNSUCCESSFUL;
    }

    origHandleEntry = *origHandleTableEntry;
    *origHandleTableEntry = NULL;

    --sectionObjectType->TotalNumberOfHandles;

    return STATUS_SUCCESS;
}

NTSTATUS CommSharedMemoryHider::removeVad()
{
    auto const process = PsGetCurrentProcess();

    auto const mem = getMemory();
    auto const tree = symbols->get<_RTL_AVL_TREE, &NT_OFFSETS::epVadRoot>(process);
    auto const vpn = reinterpret_cast<ULONG64>(mem->getContext()->kernelVirtualAddress) >> PAGE_SHIFT;
    auto const vad = findVad(tree, vpn);

    if (!MmIsAddressValid(vad))
    {
        return STATUS_NOT_FOUND;
    }

    parentVad = reinterpret_cast<_RTL_BALANCED_NODE*>(vad->Core.VadNode.ParentValue & ~0x3ULL);
    origVad = vad;

    if (parentVad && MmIsAddressValid(parentVad)) 
    {
        auto parentVadStruct = CONTAINING_RECORD(parentVad, _MMVAD_SHORT, VadNode);
        wasRightChild = (vad->Core.StartingVpn > parentVadStruct->StartingVpn);
    }
    else 
    {
        wasRightChild = FALSE;
    }

    RtlAvlRemoveNode(&tree->Root, &vad->Core.VadNode);

    if (!MmIsAddressValid(findVad(tree, vpn)))
    {
        --*symbols->get<UCHAR, &NT_OFFSETS::epVadCount>(process);
        *symbols->get<PVOID, &NT_OFFSETS::epVadHint>(process) = tree->Root;

        return STATUS_SUCCESS;
    }

    return STATUS_UNSUCCESSFUL;
}

