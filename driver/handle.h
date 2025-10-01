#pragma once

#include <cstdint>

using byte = uint8_t;
using max_align_t = double;

#include <type_traits>

#include "../shared/hash.h"

namespace memory
{
	class handle
	{
	public:
		handle(void* ptr = nullptr);
		explicit handle(std::uintptr_t ptr);

		template <typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> as();

		template <typename T>
		std::enable_if_t<std::is_lvalue_reference_v<T>, T> as();

		template <typename T>
		std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> as();

		template <typename T>
		handle add(T offset);

		template <typename T>
		handle sub(T offset);

		handle rip();

		handle getCall();

		static handle getModule(PVOID process, hash_t moduleHash);
		static handle getProc(ULONG64 base, hash_t procHash);

		explicit operator bool() const;

		friend bool operator==(handle a, handle b);
		friend bool operator!=(handle a, handle b);
	private:
		void* address;
	};

	inline handle::handle(void* ptr) :
		address(ptr)
	{
	}

	inline handle::handle(std::uintptr_t ptr) :
		address(reinterpret_cast<void*>(ptr))
	{
	}

	template <typename T>
	inline std::enable_if_t<std::is_pointer_v<T>, T> handle::as()
	{
		return static_cast<T>(address);
	}

	template <typename T>
	inline std::enable_if_t<std::is_lvalue_reference_v<T>, T> handle::as()
	{
		return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(address);
	}

	template <typename T>
	inline std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> handle::as()
	{
		return reinterpret_cast<std::uintptr_t>(address);
	}

	template <typename T>
	inline handle handle::add(T offset)
	{
		if (!this->address)
		{
			return { nullptr };
		}

		return handle(as<std::uintptr_t>() + offset);
	}

	template <typename T>
	inline handle handle::sub(T offset)
	{
		if (!this->address)
			return { nullptr };
		return handle(as<std::uintptr_t>() - offset);
	}

	inline handle handle::rip()
	{
		if (!this->address)
			return { nullptr };
		return add(as<std::int32_t&>()).add(4);
	}

	inline handle handle::getCall()
	{
		if (!this->address)
			return { nullptr };
		return handle(*reinterpret_cast<int*>(as<std::uintptr_t>() + 1) + as<std::uintptr_t>() + 5);
	}

	inline bool operator==(handle a, handle b) {
		return a.address == b.address;
	}

	inline bool operator!=(handle a, handle b) {
		return a.address != b.address;
	}

	inline handle::operator bool() const
	{
		return address != 0;

	}
}
