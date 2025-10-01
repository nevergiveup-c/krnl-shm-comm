#pragma once
#include <ntdef.h>

#include "sharedSymbol.h"

struct NT_OFFSETS;

class Symbols
{
    auto static constexpr nan = 0xDEADBEEF;

public:
    Symbols(const HANDLE pid, const PVOID data, const SIZE_T size) {
	    initialize(pid, data, size);
    }

    bool initialize(HANDLE pid, PVOID data, SIZE_T size);

    template <class Type, ULONGLONG NT_OFFSETS::* Field>
    Type* get(PVOID base);

    template <class Type, ULONGLONG NT_OFFSETS::* Field>
    bool set(PVOID base, const Type& val);

    template <ULONGLONG NT_OFFSETS::*Field>
    bool hasOffset() const;

    NT_OFFSETS& data() { return offsets; }

private:
    NT_OFFSETS offsets{};
};

template <class Type, ULONGLONG NT_OFFSETS::* Field>
Type* Symbols::get(PVOID base)
{
    if (base == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<Type*>(reinterpret_cast<ULONGLONG>(base) +
        static_cast<ULONGLONG>(offsets.*Field));
}

template <class Type, ULONGLONG NT_OFFSETS::* Field>
bool Symbols::set(PVOID base, const Type& val)
{
    if (base == nullptr)
    {
        return false;
    }

    auto v = reinterpret_cast<Type*>(reinterpret_cast<ULONGLONG>(base) +
        static_cast<ULONGLONG>(offsets.*Field));

    *v = val;
    return true;
}

template<ULONGLONG NT_OFFSETS::* Field>
inline bool Symbols::hasOffset() const
{
    return static_cast<ULONGLONG>(offsets.*Field) != static_cast<ULONGLONG>(nan);
}

inline Symbols* symbols{ nullptr };

#define LOAD_NT_OFFSETS(pid, data, size) symbols = new Symbols{ pid, data, size };