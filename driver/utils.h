// ReSharper disable CppClangTidyReadabilityInconsistentDeclarationParameterName
// ReSharper disable CppClangTidyClangDiagnosticMicrosoftExceptionSpec
// ReSharper disable CppClangTidyBugproneMacroParentheses

#pragma once
#include <ntdef.h>
#define OBJECT_TO_OBJECT_HEADER(Object) \
	reinterpret_cast<_OBJECT_HEADER*>(reinterpret_cast<char*>(Object) - offsetof(_OBJECT_HEADER, Body));

#define OBJECT_HEADER_TO_OBJECT(Object, ObjectType) \
	reinterpret_cast<ObjectType*>(reinterpret_cast<char*>(Object) + offsetof(_OBJECT_HEADER, Body));

#define CONTAINING_RECORD_BY_OFFSET(address, type, offset) ((type *)( \
                                                  (PCHAR)(address) - \
                                                  (offset)))

__int64 __fastcall expLookupHandleTableEntry(unsigned int* a1, __int64 a2);

PVOID getBaseAddress(const char* driver);

struct SectionData
{
	ULONG64 mBegin{};
	ULONG mSize{};
};

SectionData getSection(ULONG64 base, const char* name);

void receivedDataFromEntry(HANDLE pid, PVOID data, PVOID output);

class ProcessHandle
{
public:
	explicit ProcessHandle(ULONG processId);
	~ProcessHandle();

	ProcessHandle(const ProcessHandle&) = delete;
	ProcessHandle& operator=(const ProcessHandle&) = delete;

	ProcessHandle(ProcessHandle&& other) noexcept : handle(other.handle)
	{
		other.handle = nullptr;
	}

	[[nodiscard]] HANDLE get() const { return handle; }
	[[nodiscard]] bool isValid() const { return handle != nullptr; }

private:
	HANDLE handle;
};
