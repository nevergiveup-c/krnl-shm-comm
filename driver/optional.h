#pragma once

template <class T>
class optional
{
public:
	optional() = default;
	optional(const T& val) : value(val), has_v(true) {}
	optional(const optional& other) : value(other.value), has_v(other.has_v) {}

	T& get() { return value; }
	const T& get() const { return value; }

	T* get_ptr() { return has_v ? &value : nullptr; }
	const T* get_ptr() const { return has_v ? &value : nullptr; }

	bool has_value() const { return has_v; }

	optional& operator=(const T& val) {
		value = val;
		has_v = true;
		return *this;
	}
	explicit operator bool() const { return has_v; }
	bool operator==(const T& val) const { return has_v && value == val; }
	bool operator==(const optional& other) const {
		if (has_v != other.has_v) {
			return false;
		}
		return !has_v || value == other.value;
	}
	bool operator!=(const T& val) const {
		return !(*this == val);
	}

	optional& set_if_empty(const T& val) {
		if (!has_v) {
			value = val;
		}
		return *this;
	}
	T value_or(const T& default_val) const {
		return has_v ? value : default_val;
	}

private:
	T value{};
	bool has_v{ false };
};

template<typename T>
optional<T> make_optional(const T& val) {
	return optional<T>(val);
}

template<typename T>
optional<T> nullopt() {
	return optional<T>();
}
