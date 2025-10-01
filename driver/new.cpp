#include "new.h"

#include <wdm.h>

void* operator new(size_t size) noexcept
{
    if (size == 0) {
        size = 1;
    }

    return ExAllocatePoolWithTag(NonPagedPool, size, 'owe');
}

void operator delete(void* ptr) noexcept
{
    if (ptr != nullptr) {
        ExFreePoolWithTag(ptr, 'owe');
    }
}

void operator delete(void* ptr, size_t) noexcept
{
    if (ptr != nullptr) {
        ExFreePoolWithTag(ptr, 'owe');
    }
}

void* operator new[](size_t size) noexcept
{
    if (size == 0) {
        size = 1;
    }

    return ExAllocatePoolWithTag(NonPagedPool, size, 'owe');
}

void operator delete[](void* ptr) noexcept
{
    if (ptr != nullptr) {
        ExFreePoolWithTag(ptr, 'owe');
    }
}

void operator delete[](void* ptr, size_t) noexcept
{
    if (ptr != nullptr) {
        ExFreePoolWithTag(ptr, 'owe');
    }
}

void* operator new(size_t, void* where) noexcept
{
    return where;
}

void operator delete(void*, void*) noexcept
{

}

void* operator new[](size_t, void* where) noexcept
{
    return where;
}

void operator delete[](void*, void*) noexcept
{

}