
#include "commSharedMemory.h"

#include <ntifs.h>

#include "framework.h"
#include "commManager.h"

NTSTATUS CommSharedMemory::mapMemory()
{
	NTSTATUS status{};
	UNICODE_STRING sectionName{};
	OBJECT_ATTRIBUTES objectAttributes{};
	LARGE_INTEGER sectionSize{};

	initSecDescriptor();

	RtlInitUnicodeString(&sectionName, SHARED_MEMORY_NAME);

	InitializeObjectAttributes(
		&objectAttributes,
		&sectionName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		&secDesc
	)

	sectionSize.QuadPart = SHARED_MEMORY_SIZE;

	status = ZwCreateSection(
		&sharedContext.sectionHandle,
		SECTION_MAP_WRITE | SECTION_MAP_READ,
		&objectAttributes,
		&sectionSize,
		PAGE_READWRITE,
		SEC_COMMIT,
		nullptr
	);

	if (dacl != nullptr)
	{
		ExFreePool(dacl);
		dacl = nullptr;
	}

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to create section: 0x%X", status);
		return status;
	}

	SIZE_T viewSize = SHARED_MEMORY_SIZE;

	status = ZwMapViewOfSection(
		sharedContext.sectionHandle,
		ZwCurrentProcess(),
		&sharedContext.kernelVirtualAddress,
		0,
		0,
		nullptr,
		&viewSize,
		ViewShare,
		0,
		PAGE_READWRITE
	);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to map section: 0x%X", status);
		ZwClose(sharedContext.sectionHandle);
		return status;
	}

	sharedContext.sharedData = static_cast<PSHARED_DATA>(sharedContext.kernelVirtualAddress);
	RtlZeroMemory(sharedContext.sharedData, SHARED_MEMORY_SIZE);

	LOG("Shared memory created successfully at 0x%p",
		sharedContext.kernelVirtualAddress);

	return STATUS_SUCCESS;
}

NTSTATUS CommSharedMemory::unmapMemory()
{
	NTSTATUS status{};

	if (sharedContext.kernelVirtualAddress != nullptr)
	{
		status = ZwUnmapViewOfSection(ZwCurrentProcess(), sharedContext.kernelVirtualAddress);

		if (!NT_SUCCESS(status))
		{
			LOG("Failed to unmap section: 0x%X", status);
			return status;
		}

		sharedContext.kernelVirtualAddress = nullptr;
		sharedContext.sharedData = nullptr;
	}

	if (sharedContext.sectionHandle != nullptr)
	{
		status = ZwMakeTemporaryObject(sharedContext.sectionHandle);

		if (!NT_SUCCESS(status))
		{
			LOG("Failed to make temp handle: 0x%X", status);
		}

		status = ZwClose(sharedContext.sectionHandle);

		if (!NT_SUCCESS(status))
		{
			LOG("Failed to close section handle: 0x%X", status);
			return status;
		}

		sharedContext.sectionHandle = nullptr;
	}

	LOG("Shared memory unmapped successfully");

	return STATUS_SUCCESS;
}

NTSTATUS CommSharedMemory::initSecDescriptor()
{
	NTSTATUS status{};
	ULONG daclLength{};

	LOG("Initializing security descriptor");

	status = RtlCreateSecurityDescriptor(&secDesc, SECURITY_DESCRIPTOR_REVISION);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to create security descriptor: 0x%X", status);
		return status;
	}

	LOG("Security descriptor created successfully");

	daclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) * 3 + RtlLengthSid(SeExports->SeLocalSystemSid) + RtlLengthSid(SeExports->SeAliasAdminsSid) +
		RtlLengthSid(SeExports->SeWorldSid);

	LOG("Calculated DACL length: %lu bytes", daclLength);

	dacl = static_cast<PACL>(ExAllocatePoolWithTag(PagedPool, daclLength, 'comm'));

	if (dacl == nullptr)
	{
		LOG("Failed to allocate DACL memory");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	LOG("DACL memory allocated at 0x%p", dacl);

	status = RtlCreateAcl(static_cast<PACL>(dacl), daclLength, ACL_REVISION);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to create ACL: 0x%X", status);
		ExFreePool(dacl);
		return status;
	}

	LOG("ACL created successfully");

	status = RtlAddAccessAllowedAce(static_cast<PACL>(dacl), ACL_REVISION, FILE_ALL_ACCESS, SeExports->SeWorldSid);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to add World SID ACE: 0x%X", status);
		ExFreePool(dacl);
		return status;
	}

	LOG("World SID ACE added successfully");

	status = RtlAddAccessAllowedAce(static_cast<PACL>(dacl), ACL_REVISION, FILE_ALL_ACCESS, SeExports->SeAliasAdminsSid);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to add Admins SID ACE: 0x%X", status);
		ExFreePool(dacl);
		return status;
	}

	LOG("Admins SID ACE added successfully");

	status = RtlAddAccessAllowedAce(static_cast<PACL>(dacl), ACL_REVISION, FILE_ALL_ACCESS, SeExports->SeLocalSystemSid);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to add System SID ACE: 0x%X", status);
		ExFreePool(dacl);
		return status;
	}

	LOG("System SID ACE added successfully");

	status = RtlSetDaclSecurityDescriptor(&secDesc, TRUE, static_cast<PACL>(dacl), FALSE);

	if (!NT_SUCCESS(status))
	{
		LOG("Failed to set DACL to security descriptor: 0x%X", status);
		ExFreePool(dacl);
		return status;
	}

	LOG("Security descriptor initialized successfully");

	return status;
}

PSHARED_CONTEXT CommSharedMemory::getContext()
{
	return &sharedContext;
}

bool CommSharedMemory::isMemoryValid() const
{
	return
		sharedContext.kernelVirtualAddress != nullptr &&
		sharedContext.sharedData != nullptr &&
		sharedContext.sectionHandle != nullptr;
}
