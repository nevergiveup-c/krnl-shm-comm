#pragma once
#include <ntdef.h>

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER Reserved[3];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE ProcessId;
    HANDLE InheritedFromProcessId;
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
    HANDLE Section;
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

struct MYCLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
}; typedef MYCLIENT_ID* PMYCLIENT_ID;

typedef struct MYKTHREAD
{
    UCHAR Header[24];
    UCHAR padding[0x5C];

    union
    {
        struct
        {
            ULONG AutoBoostActive : 1;                                        //0x74
            ULONG ReadyTransition : 1;                                        //0x74
            ULONG WaitNext : 1;                                               //0x74
            ULONG SystemAffinityActive : 1;                                   //0x74
            ULONG Alertable : 1;                                              //0x74
            ULONG UserStackWalkActive : 1;                                    //0x74
            ULONG ApcInterruptRequest : 1;                                    //0x74
            ULONG QuantumEndMigrate : 1;                                      //0x74
            ULONG SecureThread : 1;                                           //0x74
            ULONG TimerActive : 1;                                            //0x74
            ULONG SystemThread : 1;                                           //0x74
            ULONG ProcessDetachActive : 1;                                    //0x74
            ULONG CalloutActive : 1;                                          //0x74
            ULONG ScbReadyQueue : 1;                                          //0x74
            ULONG ApcQueueable : 1;                                           //0x74
            ULONG ReservedStackInUse : 1;                                     //0x74
            ULONG Spare : 1;                                                  //0x74
            ULONG TimerSuspended : 1;                                         //0x74
            ULONG SuspendedWaitMode : 1;                                      //0x74
            ULONG SuspendSchedulerApcWait : 1;                                //0x74
            ULONG CetUserShadowStack : 1;                                     //0x74
            ULONG BypassProcessFreeze : 1;                                    //0x74
            ULONG CetKernelShadowStack : 1;                                   //0x74
            ULONG StateSaveAreaDecoupled : 1;                                 //0x74
            ULONG Reserved : 8;                                               //0x74
        };
        LONG MiscFlags;                                                     //0x74
    };
    union
    {
        struct
        {
            ULONG UserIdealProcessorFixed : 1;                                //0x78
            ULONG IsolationWidth : 1;                                         //0x78
            ULONG AutoAlignment : 1;                                          //0x78
            ULONG DisableBoost : 1;                                           //0x78
            ULONG AlertedByThreadId : 1;                                      //0x78
            ULONG QuantumDonation : 1;                                        //0x78
            ULONG EnableStackSwap : 1;                                        //0x78
            ULONG GuiThread : 1;                                              //0x78
            ULONG DisableQuantum : 1;                                         //0x78
            ULONG ChargeOnlySchedulingGroup : 1;                              //0x78
            ULONG DeferPreemption : 1;                                        //0x78
            ULONG QueueDeferPreemption : 1;                                   //0x78
            ULONG ForceDeferSchedule : 1;                                     //0x78
            ULONG SharedReadyQueueAffinity : 1;                               //0x78
            ULONG FreezeCount : 1;                                            //0x78
            ULONG TerminationApcRequest : 1;                                  //0x78
            ULONG AutoBoostEntriesExhausted : 1;                              //0x78
            ULONG KernelStackResident : 1;                                    //0x78
            ULONG TerminateRequestReason : 2;                                 //0x78
            ULONG ProcessStackCountDecremented : 1;                           //0x78
            ULONG RestrictedGuiThread : 1;                                    //0x78
            ULONG VpBackingThread : 1;                                        //0x78
            ULONG EtwStackTraceCrimsonApcDisabled : 1;                        //0x78
            ULONG EtwStackTraceApcInserted : 8;                               //0x78
        };
        volatile LONG ThreadFlags;                                            //0x78
    };

    UCHAR padding2[0x278];

    _LIST_ENTRY ThreadListEntry;

    UCHAR padding3[0x178];
}; typedef MYKTHREAD* PMYKTHREAD;

struct MYTHREAD
{
    MYKTHREAD Tcb;
    UCHAR padding[0x20];
    PVOID StartAddress;
    UCHAR padding2[0x20];
    MYCLIENT_ID Cid;
    UCHAR padding3[0x48];
    PVOID Win32StartAddress;
    UCHAR padding4[0x10];
	_LIST_ENTRY ThreadListEntry;
}; typedef MYTHREAD* PMYTHREAD;

struct MYKPROCESS
{
    UCHAR padding[0x30];
    _LIST_ENTRY ThreadListHead;
}; typedef MYKPROCESS* PMYKPROCESS;

struct MYPROCESS
{
    MYKPROCESS Pcb;
    UCHAR padding1[0x5A0];
    _LIST_ENTRY ThreadListHead;
}; typedef MYPROCESS* PMYPROCESS;

struct _EXHANDLE
{
    union
    {
        struct
        {
            ULONG TagBits : 2;
            ULONG Index : 30;
        };
        VOID* GenericHandleOverlay;
        ULONGLONG Value;
    };
};

struct _EX_PUSH_LOCK
{
    union
    {
        struct
        {
            ULONGLONG Locked : 1;
            ULONGLONG Waiting : 1;
            ULONGLONG Waking : 1;
            ULONGLONG MultipleShared : 1;
            ULONGLONG Shared : 60;
        };
        ULONGLONG Value;
        VOID* Ptr;
    };
};

union _HANDLE_TABLE_ENTRY
{
    volatile LONGLONG VolatileLowValue;
    LONGLONG LowValue;
    struct
    {
        struct _HANDLE_TABLE_ENTRY_INFO* volatile InfoTable;
        LONGLONG HighValue;
        union _HANDLE_TABLE_ENTRY* NextFreeHandleEntry;
        struct _EXHANDLE LeafHandleValue;
    };
    LONGLONG RefCountField;
    ULONGLONG Unlocked : 1;
    ULONGLONG RefCnt : 16;
    ULONGLONG Attributes : 3;
    struct
    {
        ULONGLONG ObjectPointerBits : 44;
        ULONG GrantedAccessBits : 25;
        ULONG NoRightsUpgrade : 1;
        ULONG Spare1 : 6;
    };
    ULONG Spare2;
};

struct _HANDLE_TABLE_FREE_LIST
{
	_EX_PUSH_LOCK FreeListLock;
	_HANDLE_TABLE_ENTRY* FirstFreeHandleEntry;
	_HANDLE_TABLE_ENTRY* LastFreeHandleEntry;
    LONG HandleCount;
    ULONG HighWaterMark;
};

struct _HANDLE_TABLE
{
    ULONG NextHandleNeedingPool;
    LONG ExtraInfoPages;
    volatile ULONGLONG TableCode;
    struct _EPROCESS* QuotaProcess;
	_LIST_ENTRY HandleTableList;
    ULONG UniqueProcessId;
    union
    {
        ULONG Flags;
        struct
        {
            UCHAR StrictFIFO : 1;
            UCHAR EnableHandleExceptions : 1;
            UCHAR Rundown : 1;
            UCHAR Duplicated : 1;
            UCHAR RaiseUMExceptionOnInvalidHandleClose : 1;
        };
    };
    struct _EX_PUSH_LOCK HandleContentionEvent; 
    struct _EX_PUSH_LOCK HandleTableLock;
    union
    {
        struct _HANDLE_TABLE_FREE_LIST FreeLists[1];
        struct
        {
            UCHAR ActualEntry[32];
            struct _HANDLE_TRACE_DEBUG_INFO* DebugInfo;
        };
    };
};

struct _OBJECT_TYPE
{
    struct _LIST_ENTRY TypeList;
    struct _UNICODE_STRING Name;
    VOID* DefaultObject;
    UCHAR Index;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
	UINT8 _OBJECT_TYPE_INITIALIZER[0x78];
    struct _EX_PUSH_LOCK TypeLock;
    ULONG Key;
    struct _LIST_ENTRY CallbackList;
};

struct _OBJECT_HEADER
{
    LONGLONG PointerCount;
    union
    {
        LONGLONG HandleCount;
        VOID* NextToFree;
    };
    struct _EX_PUSH_LOCK Lock;
    UCHAR TypeIndex;
    union
    {
        UCHAR TraceFlags;
        struct
        {
            UCHAR DbgRefTrace : 1;
            UCHAR DbgTracePermanent : 1;
        };
    };
    UCHAR InfoMask;
    union
    {
        UCHAR Flags;
        struct
        {
            UCHAR NewObject : 1;
            UCHAR KernelObject : 1;
            UCHAR KernelOnlyAccess : 1;
            UCHAR ExclusiveObject : 1;
            UCHAR PermanentObject : 1;
            UCHAR DefaultSecurityQuota : 1;
            UCHAR SingleHandleEntry : 1;
            UCHAR DeletedInline : 1;
        };
    };
    ULONG Reserved;
    union
    {
        struct _OBJECT_CREATE_INFORMATION* ObjectCreateInfo;
        VOID* QuotaBlockCharged;
    };
    VOID* SecurityDescriptor;
    struct _QUAD Body;
};

struct _OBJECT_HEADER_CREATOR_INFO
{
    struct _LIST_ENTRY TypeList;                                            //0x0
    VOID* CreatorUniqueProcess;                                             //0x10
    USHORT CreatorBackTraceIndex;                                           //0x18
    USHORT Reserved1;                                                       //0x1a
    ULONG Reserved2;                                                        //0x1c
};

struct _MMSECTION_FLAGS
{
    ULONG BeingDeleted : 1;                                                   //0x0
    ULONG BeingCreated : 1;                                                   //0x0
    ULONG BeingPurged : 1;                                                    //0x0
    ULONG NoModifiedWriting : 1;                                              //0x0
    ULONG FailAllIo : 1;                                                      //0x0
    ULONG Image : 1;                                                          //0x0
    ULONG Based : 1;                                                          //0x0
    ULONG File : 1;                                                           //0x0
    ULONG SectionOfInterest : 1;                                              //0x0
    ULONG PrefetchCreated : 1;                                                //0x0
    ULONG PhysicalMemory : 1;                                                 //0x0
    ULONG ImageControlAreaOnRemovableMedia : 1;                               //0x0
    ULONG Reserve : 1;                                                        //0x0
    ULONG Commit : 1;                                                         //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG WasPurged : 1;                                                      //0x0
    ULONG UserReference : 1;                                                  //0x0
    ULONG GlobalMemory : 1;                                                   //0x0
    ULONG DeleteOnClose : 1;                                                  //0x0
    ULONG FilePointerNull : 1;                                                //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG GlobalOnlyPerSession : 1;                                           //0x0
    ULONG ControlAreaOnUnusedList : 1;                                        //0x0
    ULONG SystemVaAllocated : 1;                                              //0x0
    ULONG NotBeingUsed : 1;                                                   //0x0
    ULONG PageSize64K : 1;                                                    //0x0
};

struct _SECTION
{
    struct _RTL_BALANCED_NODE SectionNode;                                  //0x0
    ULONGLONG StartingVpn;                                                  //0x18
    ULONGLONG EndingVpn;                                                    //0x20
    union
    {
        struct _CONTROL_AREA* ControlArea;                                  //0x28
        struct _FILE_OBJECT* FileObject;                                    //0x28
        ULONGLONG RemoteImageFileObject : 1;                                  //0x28
        ULONGLONG RemoteDataFileObject : 1;                                   //0x28
    } u1;                                                                   //0x28
    ULONGLONG SizeOfSection;                                                //0x30
    union
    {
        ULONG LongFlags;                                                    //0x38
        struct _MMSECTION_FLAGS Flags;                                      //0x38
    } u;                                                                    //0x38
    ULONG InitialPageProtection : 12;                                         //0x3c
    ULONG SessionId : 19;                                                     //0x3c
    ULONG NoValidationNeeded : 1;                                             //0x3c
};

struct _OBJECT_SYMBOLIC_LINK
{
    union _LARGE_INTEGER CreationTime;                                      //0x0
    union
    {
        struct _UNICODE_STRING LinkTarget;                                  //0x8
        struct
        {
            LONG(*Callback)(struct _OBJECT_SYMBOLIC_LINK* arg1, VOID* arg2, struct _UNICODE_STRING* arg3, VOID** arg4); //0x8
            VOID* CallbackContext;                                          //0x10
        };
    };
    ULONG DosDeviceDriveIndex;                                              //0x18
    ULONG Flags;                                                            //0x1c
    ULONG AccessMask;                                                       //0x20
    ULONG IntegrityLevel;                                                   //0x24
};

struct _OBJECT_DIRECTORY
{
    struct _OBJECT_DIRECTORY_ENTRY* HashBuckets[37];                        //0x0
    struct _EX_PUSH_LOCK Lock;                                              //0x128
    struct _DEVICE_MAP* DeviceMap;                                          //0x130
    struct _OBJECT_DIRECTORY* ShadowDirectory;                              //0x138
    VOID* NamespaceEntry;                                                   //0x140
    VOID* SessionObject;                                                    //0x148
    ULONG Flags;                                                            //0x150
    ULONG SessionId;                                                        //0x154
};

struct _OBJECT_DIRECTORY_ENTRY
{
    struct _OBJECT_DIRECTORY_ENTRY* ChainLink;                              //0x0
    VOID* Object;                                                           //0x8
    ULONG HashValue;                                                        //0x10
};

struct _OBJECT_HEADER_NAME_INFO
{
    struct _OBJECT_DIRECTORY* Directory;                                    //0x0
    struct _UNICODE_STRING Name;                                            //0x8
    LONG ReferenceCount;                                                    //0x18
    ULONG Reserved;                                                         //0x1c
};

struct _MMVAD_FLAGS
{
    union
    {
        struct
        {
            ULONG Lock : 1;                                                   //0x0
            ULONG LockContended : 1;                                          //0x0
            ULONG DeleteInProgress : 1;                                       //0x0
            ULONG NoChange : 1;                                               //0x0
            ULONG VadType : 3;                                                //0x0
            ULONG Protection : 5;                                             //0x0
            ULONG PreferredNode : 7;                                          //0x0
            ULONG PageSize : 2;                                               //0x0
            ULONG PrivateMemory : 1;                                          //0x0
        };
        ULONG EntireField;                                                  //0x0
    };
};

//0x4 bytes (sizeof)
struct _MM_PRIVATE_VAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemoryAlwaysSet : 1;                                         //0x0
    ULONG WriteWatch : 1;                                                     //0x0
    ULONG FixedLargePageSize : 1;                                             //0x0
    ULONG ZeroFillPagesOptional : 1;                                          //0x0
    ULONG MemCommit : 1;                                                      //0x0
    ULONG Graphics : 1;                                                       //0x0
    ULONG Enclave : 1;                                                        //0x0
    ULONG ShadowStack : 1;                                                    //0x0
    ULONG PhysicalMemoryPfnsReferenced : 1;                                   //0x0
};

struct _MM_GRAPHICS_VAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemoryAlwaysSet : 1;                                         //0x0
    ULONG WriteWatch : 1;                                                     //0x0
    ULONG FixedLargePageSize : 1;                                             //0x0
    ULONG ZeroFillPagesOptional : 1;                                          //0x0
    ULONG MemCommit : 1;                                                      //0x0
    ULONG GraphicsAlwaysSet : 1;                                              //0x0
    ULONG GraphicsUseCoherentBus : 1;                                         //0x0
    ULONG GraphicsNoCache : 1;                                                //0x0
    ULONG GraphicsPageProtection : 3;                                         //0x0
};

//0x4 bytes (sizeof)
struct _MM_SHARED_VAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemoryAlwaysClear : 1;                                       //0x0
    ULONG PrivateFixup : 1;                                                   //0x0
    ULONG HotPatchState : 2;                                                  //0x0
};

struct _MMVAD_SHORT
{
    union
    {
        struct
        {
            struct _MMVAD_SHORT* NextVad;                                   //0x0
            VOID* ExtraCreateInfo;                                          //0x8
        };
        struct _RTL_BALANCED_NODE VadNode;                                  //0x0
    };
    ULONG StartingVpn;                                                      //0x18
    ULONG EndingVpn;                                                        //0x1c
    UCHAR StartingVpnHigh;                                                  //0x20
    UCHAR EndingVpnHigh;                                                    //0x21
    UCHAR CommitChargeHigh;                                                 //0x22
    UCHAR SpareNT64VadUChar;                                                //0x23
    volatile LONG ReferenceCount;                                           //0x24
    struct _EX_PUSH_LOCK PushLock;                                          //0x28
    union
    {
        ULONG LongFlags;                                                    //0x30
        struct _MMVAD_FLAGS VadFlags;                                       //0x30
        struct _MM_PRIVATE_VAD_FLAGS PrivateVadFlags;                       //0x30
        struct _MM_GRAPHICS_VAD_FLAGS GraphicsVadFlags;                     //0x30
        struct _MM_SHARED_VAD_FLAGS SharedVadFlags;                         //0x30
        volatile ULONG VolatileVadLong;                                     //0x30
    } u;                                                                    //0x30
    ULONG CommitCharge;                                                     //0x34
    union
    {
        struct _MI_VAD_EVENT_BLOCK* EventList;                              //0x38
    } u5;                                                                   //0x38
};

struct _MMVAD_FLAGS2
{
    union
    {
        struct
        {
            ULONG Large : 1;                                                  //0x0
            ULONG TrimBehind : 1;                                             //0x0
            ULONG Inherit : 1;                                                //0x0
            ULONG NoValidationNeeded : 1;                                     //0x0
            ULONG PrivateDemandZero : 1;                                      //0x0
            ULONG ImageMappingExtended : 1;                                   //0x0
            ULONG Spare : 26;                                                 //0x0
        };
        ULONG LongFlags;                                                    //0x0
    };
};

//0x8 bytes (sizeof)
struct _MI_VAD_SEQUENTIAL_INFO
{
    union
    {
        struct
        {
            ULONGLONG Length : 12;                                            //0x0
            ULONGLONG Vpn : 51;                                               //0x0
            ULONGLONG MustBeZero : 1;                                         //0x0
        };
        ULONGLONG EntireField;                                              //0x0
    };
};

struct _MMVAD
{
    struct _MMVAD_SHORT Core;                                               //0x0
    struct _MMVAD_FLAGS2 VadFlags2;                                         //0x40
    struct _SUBSECTION* Subsection;                                         //0x48
    struct _MMPTE* FirstPrototypePte;                                       //0x50
    struct _MMPTE* LastContiguousPte;                                       //0x58
    struct _LIST_ENTRY ViewLinks;                                           //0x60
    struct _EPROCESS* VadsProcess;                                          //0x70
    union
    {
        struct _MI_VAD_SEQUENTIAL_INFO SequentialVa;                        //0x78
        struct _MMEXTEND_INFO* ExtendedInfo;                                //0x78
    } u4;                                                                   //0x78
    struct _FILE_OBJECT* FileObject;                                        //0x80
};

struct _RTL_AVL_TREE
{
    struct _RTL_BALANCED_NODE* Root;                                        //0x0
};

struct _KAFFINITY_EX
{
    USHORT Count;                                                           //0x0
    USHORT Size;                                                            //0x2
    ULONG Reserved;                                                         //0x4
    union
    {
        ULONGLONG Bitmap[1];                                                //0x8
        ULONGLONG StaticBitmap[32];                                         //0x8
    };
};

struct _PEB_LDR_DATA
{
    ULONG Length;                                                           //0x0
    UCHAR Initialized;                                                      //0x4
    VOID* SsHandle;                                                         //0x8
    struct _LIST_ENTRY InLoadOrderModuleList;                               //0x10
    struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x20
    struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x30
    VOID* EntryInProgress;                                                  //0x40
    UCHAR ShutdownInProgress;                                               //0x48
    VOID* ShutdownThreadId;                                                 //0x50
};

struct _LDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
    VOID* DllBase;                                                          //0x30
    VOID* EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    struct _UNICODE_STRING FullDllName;                                     //0x48
    struct _UNICODE_STRING BaseDllName;                                     //0x58
    union
    {
        UCHAR FlagGroup[4];                                                 //0x68
        ULONG Flags;                                                        //0x68
        struct
        {
            ULONG PackagedBinary : 1;                                         //0x68
            ULONG MarkedForRemoval : 1;                                       //0x68
            ULONG ImageDll : 1;                                               //0x68
            ULONG LoadNotificationsSent : 1;                                  //0x68
            ULONG TelemetryEntryProcessed : 1;                                //0x68
            ULONG ProcessStaticImport : 1;                                    //0x68
            ULONG InLegacyLists : 1;                                          //0x68
            ULONG InIndexes : 1;                                              //0x68
            ULONG ShimDll : 1;                                                //0x68
            ULONG InExceptionTable : 1;                                       //0x68
            ULONG VerifierProvider : 1;                                       //0x68
            ULONG ShimEngineCalloutSent : 1;                                  //0x68
            ULONG LoadInProgress : 1;                                         //0x68
            ULONG LoadConfigProcessed : 1;                                    //0x68
            ULONG EntryProcessed : 1;                                         //0x68
            ULONG ProtectDelayLoad : 1;                                       //0x68
            ULONG AuxIatCopyPrivate : 1;                                      //0x68
            ULONG ReservedFlags3 : 1;                                         //0x68
            ULONG DontCallForThreads : 1;                                     //0x68
            ULONG ProcessAttachCalled : 1;                                    //0x68
            ULONG ProcessAttachFailed : 1;                                    //0x68
            ULONG ScpInExceptionTable : 1;                                    //0x68
            ULONG CorImage : 1;                                               //0x68
            ULONG DontRelocate : 1;                                           //0x68
            ULONG CorILOnly : 1;                                              //0x68
            ULONG ChpeImage : 1;                                              //0x68
            ULONG ChpeEmulatorImage : 1;                                      //0x68
            ULONG ReservedFlags5 : 1;                                         //0x68
            ULONG Redirected : 1;                                             //0x68
            ULONG ReservedFlags6 : 2;                                         //0x68
            ULONG CompatDatabaseProcessed : 1;                                //0x68
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x6c
    USHORT TlsIndex;                                                        //0x6e
    struct _LIST_ENTRY HashLinks;                                           //0x70
    ULONG TimeDateStamp;                                                    //0x80
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
    VOID* Lock;                                                             //0x90
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x98
    struct _LIST_ENTRY NodeModuleLink;                                      //0xa0
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0
    VOID* ParentDllBase;                                                    //0xb8
    VOID* SwitchBackContext;                                                //0xc0
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0
    ULONGLONG OriginalBase;                                                 //0xf8
    union _LARGE_INTEGER LoadTime;                                          //0x100
    ULONG BaseNameHashValue;                                                //0x108
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c
    ULONG ImplicitPathOptions;                                              //0x110
    ULONG ReferenceCount;                                                   //0x114
    ULONG DependentLoadFlags;                                               //0x118
    UCHAR SigningLevel;                                                     //0x11c
    ULONG CheckSum;                                                         //0x120
    VOID* ActivePatchImageBase;                                             //0x128
    enum _LDR_HOT_PATCH_STATE HotPatchState;                                //0x130
};