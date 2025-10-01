// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
// ReSharper disable CppClangTidyClangDiagnosticMicrosoftExceptionSpec

#pragma once

void* operator new(size_t size) noexcept;
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, size_t) noexcept;
void* operator new[](size_t size) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, size_t) noexcept;
void* operator new(size_t, void* where) noexcept;
void operator delete(void*, void*) noexcept;
void* operator new[](size_t, void* where) noexcept;
void operator delete[](void*, void*) noexcept;
