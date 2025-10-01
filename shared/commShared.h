// ReSharper disable CppClangTidyClangDiagnosticNestedAnonTypes
// ReSharper disable CppClangTidyPerformanceEnumSize

#pragma once
constexpr auto SHARED_MEMORY_NAME = L"\\BaseNamedObjects\\Shared";
constexpr auto SHARED_MEMORY_CLIENT_NAME = L"Global\\Shared";
constexpr auto SHARED_MEMORY_SIZE = 0x1000;

enum COMMANDS
{
    CMD_NONE = 0,
    CMD_SHUTDOWN,
    CMD_READ_MEMORY,
    CMD_WRITE_MEMORY,
    CMD_ALLOCATE_MEMORY,
    CMD_FREE_MEMORY,
    CMD_MAX
};

enum COMMAND_STATUS
{
    STATUS_WAIT = 0,
    STATUS_PROCESSING,
    STATUS_COMPLETED,
    STATUS_ERROR
};

enum CLIENT_STATUS
{
	STATUS_CONNECTION = 0,
	STATUS_CONNECTED,
	STATUS_SHUTDOWN,
};

typedef struct SHARED_DATA
{
    COMMANDS command{};
    ULONG processId;
    NTSTATUS result;
    CLIENT_STATUS clientStatus;
    COMMAND_STATUS commandStatus;

    union
	{
        struct
    	{
            PVOID targetAddress;
            PVOID buffer;
            SIZE_T size;
        } memory;

        struct
    	{
            PVOID address;
            SIZE_T size;
            ULONG newProtection;
            ULONG oldProtection;
        } protection;

        struct
    	{
            SIZE_T size;
            ULONG allocationType;
            ULONG protection;
            PVOID allocatedAddress;
        } allocation;

        struct
        {
            PVOID address;
            SIZE_T size;
            ULONG freeType;
        } free;
    };

    SIZE_T dataSize{};
    UCHAR dataBuffer[1];

} *PSHARED_DATA;

typedef struct SHARED_CONTEXT
{
    HANDLE sectionHandle;
    PVOID kernelVirtualAddress;
    PSHARED_DATA sharedData;
} *PSHARED_CONTEXT;
