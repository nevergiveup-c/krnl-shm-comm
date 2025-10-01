#pragma once

template <class T, class B>
struct pair
{
	T m_first{};
	B m_second{};
	pair(const T& first, const B& second) : m_first(first), m_second(second) {}

	bool operator==(const pair& p) const
	{
		return m_first == p.m_first && m_second == p.m_second;
	}
};