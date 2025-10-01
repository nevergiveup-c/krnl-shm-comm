#pragma once
#include <ntdef.h>

/**
 * @note
 *  kt=_KTHREAD
 *  et=_ETHREAD\n
 *  kp=_KPROCESS
 *  ep=_EPROCESS
 */
typedef struct NT_OFFSETS
{
    ULONGLONG ktMiscFlags{};
    ULONGLONG ktThreadFlags{};
    ULONGLONG etThreadListEntry{};
    ULONGLONG etStartAddress{};
    ULONGLONG etWin32StartAddress{};
    ULONGLONG epObjectTable{};
    ULONGLONG epThreadListHead{};
    ULONGLONG epActiveThreads{};
    ULONGLONG epVadRoot{};
    ULONGLONG epVadHint{};
    ULONGLONG epVadCount{};
    ULONGLONG epPeb{};
    ULONGLONG etCid{};
    ULONGLONG gPspCidTable{};
    ULONGLONG gObpKernelHandleTable{};
    ULONGLONG gObpObjectTypes{};
    ULONGLONG gMmSectionObjectType{};
    ULONGLONG gObHeaderCookie{};
    ULONGLONG gObpSymbolicLinkObjectType{};
    ULONGLONG gObpRootDirectoryObject{};
    ULONGLONG gObpInfoMaskToOffset{};
    ULONGLONG fEtwpLogger{};
    ULONGLONG fKeGetCurrentPrcb{};
}*PNT_OFFSETS;