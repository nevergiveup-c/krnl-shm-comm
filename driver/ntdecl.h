#pragma once

extern "C"
{
    NTKERNELAPI NTSTATUS NTAPI MmCopyVirtualMemory(
        PEPROCESS SourceProcess,
        PVOID SourceAddress,
        PEPROCESS TargetProcess,
        PVOID TargetAddress,
        SIZE_T BufferSize,
        KPROCESSOR_MODE PreviousMode,
        PSIZE_T ReturnSize
    );

    NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(
    enum _SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength OPTIONAL
    );

    VOID __fastcall RtlAvlRemoveNode(
        struct _RTL_BALANCED_NODE** root,
        struct _RTL_BALANCED_NODE* node
    );

    VOID __fastcall RtlAvlInsertNodeEx(
        struct _RTL_AVL_TREE* Tree,
        struct _RTL_BALANCED_NODE* Parent,
        BOOLEAN Right,
        struct _RTL_BALANCED_NODE* Node
    );

    NTSYSAPI
    NTSTATUS
    NTAPI
    ZwProtectVirtualMemory(
        _In_ HANDLE ProcessHandle,
        _Inout_ PVOID* BaseAddress,
        _Inout_ PSIZE_T RegionSize,
        _In_ ULONG NewProtect,
        _Out_ PULONG OldProtect
    );
}