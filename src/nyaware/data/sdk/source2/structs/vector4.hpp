#pragma once

#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <limits>
#include <cstdio>

#include "vector3.hpp"

struct vector4_t {
	float x{}, y{}, z{}, w{};

	constexpr vector4_t() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	constexpr vector4_t(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
	constexpr vector4_t(float value) noexcept : x(value), y(value), z(value), w(value) {}
	constexpr vector4_t(const vector3_t& xyz, float w) noexcept : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
	constexpr vector4_t(float x, float y, float z) noexcept : x(x), y(y), z(z), w(0.0f) {}
	constexpr vector4_t(const float* data) noexcept : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}

	static constexpr vector4_t zero() noexcept { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
	static constexpr vector4_t one() noexcept { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
	static constexpr vector4_t black() noexcept { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
	static constexpr vector4_t white() noexcept { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
	static constexpr vector4_t red() noexcept { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
	static constexpr vector4_t green() noexcept { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
	static constexpr vector4_t blue() noexcept { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
	static constexpr vector4_t yellow() noexcept { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
	static constexpr vector4_t cyan() noexcept { return { 0.0f, 1.0f, 1.0f, 1.0f }; }
	static constexpr vector4_t magenta() noexcept { return { 1.0f, 0.0f, 1.0f, 1.0f }; }
	static constexpr vector4_t clear() noexcept { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
	static constexpr vector4_t gray() noexcept { return { 0.5f, 0.5f, 0.5f, 1.0f }; }

	float magnitude() const noexcept;
	float sqr_magnitude() const noexcept;

	void normalize() noexcept;
	vector4_t normalized() const noexcept;
	float dot(const vector4_t& other) const noexcept;
	float distance(const vector4_t& other) const noexcept;

	vector4_t lerp(const vector4_t& other, float t) const noexcept;
	vector4_t lerp_unclamped(const vector4_t& other, float t) const noexcept;

	bool has_no_zero() const noexcept;
	bool is_approximately_zero(float epsilon = 1e-6f) const noexcept;
	bool is_approximately_equal(const vector4_t& other, float epsilon = 1e-6f) const noexcept;

	vector4_t abs() const noexcept;
	vector4_t floor() const noexcept;
	vector4_t ceil() const noexcept;
	vector4_t round() const noexcept;
	vector4_t sign() const noexcept;
	vector4_t clamp_components(float min_val, float max_val) const noexcept;

	vector4_t clamp_magnitude(float max_length) const noexcept;
	void clamp_magnitude_in_place(float max_length) noexcept;

	vector4_t scale(const vector4_t& other) const noexcept;

	std::string to_string() const;

	vector3_t xyz() const noexcept;
	vector3_t homogenized() const noexcept;

	bool is_point() const noexcept;
	bool is_direction() const noexcept;
	vector4_t as_point() const noexcept;
	vector4_t as_direction() const noexcept;

	void set(float new_x, float new_y, float new_z, float new_w) noexcept;
	void set(const vector3_t& xyz, float new_w) noexcept;

	static float dot(const vector4_t& a, const vector4_t& b) noexcept;
	static float magnitude(const vector4_t& v) noexcept;
	static float sqr_magnitude(const vector4_t& v) noexcept;
	static vector4_t normalized(const vector4_t& v) noexcept;
	static float distance(const vector4_t& a, const vector4_t& b) noexcept;
	static vector4_t lerp(const vector4_t& a, const vector4_t& b, float t) noexcept;
	static vector4_t lerp_unclamped(const vector4_t& a, const vector4_t& b, float t) noexcept;
	static vector4_t scale(const vector4_t& a, const vector4_t& b) noexcept;
	static vector4_t clamp_magnitude(const vector4_t& v, float max_length) noexcept;
	static vector4_t move_towards(const vector4_t& current, const vector4_t& target, float max_distance_delta) noexcept;
	static std::string to_string(const vector4_t& v);

	float& operator[](size_t index) noexcept;
	const float& operator[](size_t index) const noexcept;

	vector4_t operator-() const noexcept;
	vector4_t& operator+=(const vector4_t& other) noexcept;
	vector4_t& operator-=(const vector4_t& other) noexcept;
	vector4_t& operator*=(float scalar) noexcept;
	vector4_t& operator/=(float scalar) noexcept;
	vector4_t& operator*=(const vector4_t& other) noexcept;
	vector4_t& operator/=(const vector4_t& other) noexcept;

	vector4_t& operator=(const vector4_t& other) noexcept = default;

	bool operator==(const vector4_t& other) const noexcept;
	bool operator!=(const vector4_t& other) const noexcept;

	bool equals(const vector4_t& other, float epsilon = 1e-6f) const noexcept;

	friend vector4_t operator+(vector4_t lhs, const vector4_t& rhs) noexcept;
	friend vector4_t operator-(vector4_t lhs, const vector4_t& rhs) noexcept;
	friend vector4_t operator*(vector4_t lhs, const vector4_t& rhs) noexcept;
	friend vector4_t operator/(vector4_t lhs, const vector4_t& rhs) noexcept;

	friend vector4_t operator+(vector4_t vec, float scalar) noexcept;
	friend vector4_t operator-(vector4_t vec, float scalar) noexcept;
	friend vector4_t operator*(vector4_t vec, float scalar) noexcept;
	friend vector4_t operator/(vector4_t vec, float scalar) noexcept;

	friend vector4_t operator+(float scalar, vector4_t vec) noexcept;
	friend vector4_t operator-(float scalar, vector4_t vec) noexcept;
	friend vector4_t operator*(float scalar, vector4_t vec) noexcept;
	friend vector4_t operator/(float scalar, vector4_t vec) noexcept;

	friend std::ostream& operator<<(std::ostream& os, const vector4_t& vec);
};

inline float vector4_t::magnitude() const noexcept {
	return std::sqrt(sqr_magnitude());
}

inline float vector4_t::sqr_magnitude() const noexcept {
	return x * x + y * y + z * z + w * w;
}

inline void vector4_t::normalize() noexcept {
	const float mag = magnitude();
	if (mag > 1e-6f) {
		*this /= mag;
	}
	else {
		*this = zero();
	}
}

inline vector4_t vector4_t::normalized() const noexcept {
	return normalized(*this);
}

inline float vector4_t::dot(const vector4_t& other) const noexcept {
	return dot(*this, other);
}

inline float vector4_t::distance(const vector4_t& other) const noexcept {
	return distance(*this, other);
}

inline vector4_t vector4_t::lerp(const vector4_t& other, float t) const noexcept {
	return lerp(*this, other, t);
}

inline vector4_t vector4_t::lerp_unclamped(const vector4_t& other, float t) const noexcept {
	return lerp_unclamped(*this, other, t);
}

inline bool vector4_t::has_no_zero() const noexcept {
	return x != 0.0f && y != 0.0f && z != 0.0f && w != 0.0f;
}

inline bool vector4_t::is_approximately_zero(float epsilon) const noexcept {
	return sqr_magnitude() < epsilon * epsilon;
}

inline bool vector4_t::is_approximately_equal(const vector4_t& other, float epsilon) const noexcept {
	return distance(other) < epsilon;
}

inline vector4_t vector4_t::abs() const noexcept {
	return {
			std::abs(x),
			std::abs(y),
			std::abs(z),
			std::abs(w)
	};
}

inline vector4_t vector4_t::floor() const noexcept {
	return {
			std::floor(x),
			std::floor(y),
			std::floor(z),
			std::floor(w)
	};
}

inline vector4_t vector4_t::ceil() const noexcept {
	return {
			std::ceil(x),
			std::ceil(y),
			std::ceil(z),
			std::ceil(w)
	};
}

inline vector4_t vector4_t::round() const noexcept {
	return {
			std::round(x),
			std::round(y),
			std::round(z),
			std::round(w)
	};
}

inline vector4_t vector4_t::sign() const noexcept {
	return {
			(x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f),
			(y > 0.0f) ? 1.0f : ((y < 0.0f) ? -1.0f : 0.0f),
			(z > 0.0f) ? 1.0f : ((z < 0.0f) ? -1.0f : 0.0f),
			(w > 0.0f) ? 1.0f : ((w < 0.0f) ? -1.0f : 0.0f)
	};
}

inline vector4_t vector4_t::clamp_components(float min_val, float max_val) const noexcept {
	return {
			std::clamp(x, min_val, max_val),
			std::clamp(y, min_val, max_val),
			std::clamp(z, min_val, max_val),
			std::clamp(w, min_val, max_val)
	};
}

inline vector4_t vector4_t::clamp_magnitude(float max_length) const noexcept {
	return clamp_magnitude(*this, max_length);
}

inline void vector4_t::clamp_magnitude_in_place(float max_length) noexcept {
	*this = clamp_magnitude(max_length);
}

inline vector4_t vector4_t::scale(const vector4_t& other) const noexcept {
	return scale(*this, other);
}

inline std::string vector4_t::to_string() const {
	return to_string(*this);
}

inline vector3_t vector4_t::xyz() const noexcept {
	return { x, y, z };
}

inline vector3_t vector4_t::homogenized() const noexcept {
	if (w != 0.0f) {
		return { x / w, y / w, z / w };
	}
	return { x, y, z };
}

inline bool vector4_t::is_point() const noexcept {
	return w == 1.0f;
}

inline bool vector4_t::is_direction() const noexcept {
	return w == 0.0f;
}

inline vector4_t vector4_t::as_point() const noexcept {
	return { x, y, z, 1.0f };
}

inline vector4_t vector4_t::as_direction() const noexcept {
	return { x, y, z, 0.0f };
}

inline void vector4_t::set(float new_x, float new_y, float new_z, float new_w) noexcept {
	x = new_x; y = new_y; z = new_z; w = new_w;
}

inline void vector4_t::set(const vector3_t& xyz, float new_w) noexcept {
	x = xyz.x; y = xyz.y; z = xyz.z; w = new_w;
}

inline float vector4_t::dot(const vector4_t& a, const vector4_t& b) noexcept {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float vector4_t::magnitude(const vector4_t& v) noexcept {
	return std::sqrt(sqr_magnitude(v));
}

inline float vector4_t::sqr_magnitude(const vector4_t& v) noexcept {
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

inline vector4_t vector4_t::normalized(const vector4_t& v) noexcept {
	const float mag = magnitude(v);
	if (mag > 1e-6f) {
		return v / mag;
	}

	return zero();
}

inline float vector4_t::distance(const vector4_t& a, const vector4_t& b) noexcept {
	return magnitude(a - b);
}

inline vector4_t vector4_t::lerp(const vector4_t& a, const vector4_t& b, float t) noexcept {
	t = std::clamp(t, 0.0f, 1.0f);
	return lerp_unclamped(a, b, t);
}

inline vector4_t vector4_t::lerp_unclamped(const vector4_t& a, const vector4_t& b, float t) noexcept {
	return {
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t,
			a.w + (b.w - a.w) * t
	};
}

inline vector4_t vector4_t::scale(const vector4_t& a, const vector4_t& b) noexcept {
	return {
			a.x * b.x,
			a.y * b.y,
			a.z * b.z,
			a.w * b.w
	};
}

inline vector4_t vector4_t::clamp_magnitude(const vector4_t& v, float max_length) noexcept {
	const float sqr_mag = sqr_magnitude(v);
	if (sqr_mag > max_length * max_length) {
		const float mag = std::sqrt(sqr_mag);
		return (v / mag) * max_length;
	}

	return v;
}

inline vector4_t vector4_t::move_towards(const vector4_t& current, const vector4_t& target, float max_distance_delta) noexcept {
	const vector4_t delta = target - current;
	const float distance = magnitude(delta);

	if (distance <= max_distance_delta || distance < 1e-6f) {
		return target;
	}

	return current + (delta / distance) * max_distance_delta;
}

inline std::string vector4_t::to_string(const vector4_t& v) {
	char buffer[80];
	std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", v.x, v.y, v.z, v.w);
	return std::string(buffer);
}

inline vector4_t vector4_t::operator-() const noexcept {
	return { -x, -y, -z, -w };
}

inline vector4_t& vector4_t::operator+=(const vector4_t& other) noexcept {
	x += other.x; y += other.y; z += other.z; w += other.w;
	return *this;
}

inline vector4_t& vector4_t::operator-=(const vector4_t& other) noexcept {
	x -= other.x; y -= other.y; z -= other.z; w -= other.w;
	return *this;
}

inline vector4_t& vector4_t::operator*=(float scalar) noexcept {
	x *= scalar; y *= scalar; z *= scalar; w *= scalar;
	return *this;
}

inline vector4_t& vector4_t::operator/=(float scalar) noexcept {
	if (scalar != 0.0f) {
		const float inv_scalar = 1.0f / scalar;
		x *= inv_scalar; y *= inv_scalar; z *= inv_scalar; w *= inv_scalar;
	}

	return *this;
}

inline vector4_t& vector4_t::operator*=(const vector4_t& other) noexcept {
	x *= other.x; y *= other.y; z *= other.z; w *= other.w;
	return *this;
}

inline vector4_t& vector4_t::operator/=(const vector4_t& other) noexcept {
	x /= other.x; y /= other.y; z /= other.z; w /= other.w;
	return *this;
}

inline bool vector4_t::operator==(const vector4_t& other) const noexcept {
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

inline bool vector4_t::operator!=(const vector4_t& other) const noexcept {
	return !(*this == other);
}

inline bool vector4_t::equals(const vector4_t& other, float epsilon) const noexcept {
	return std::abs(x - other.x) < epsilon &&
		std::abs(y - other.y) < epsilon &&
		std::abs(z - other.z) < epsilon &&
		std::abs(w - other.w) < epsilon;
}

inline vector4_t operator+(vector4_t lhs, const vector4_t& rhs) noexcept {
	lhs += rhs;
	return lhs;
}

inline vector4_t operator-(vector4_t lhs, const vector4_t& rhs) noexcept {
	lhs -= rhs;
	return lhs;
}

inline vector4_t operator*(vector4_t lhs, const vector4_t& rhs) noexcept {
	lhs *= rhs;
	return lhs;
}

inline vector4_t operator/(vector4_t lhs, const vector4_t& rhs) noexcept {
	lhs /= rhs;
	return lhs;
}

inline vector4_t operator+(vector4_t vec, float scalar) noexcept {
	return { vec.x + scalar, vec.y + scalar, vec.z + scalar, vec.w + scalar };
}

inline vector4_t operator-(vector4_t vec, float scalar) noexcept {
	return { vec.x - scalar, vec.y - scalar, vec.z - scalar, vec.w - scalar };
}

inline vector4_t operator*(vector4_t vec, float scalar) noexcept {
	vec *= scalar;
	return vec;
}

inline vector4_t operator/(vector4_t vec, float scalar) noexcept {
	if (scalar != 0.0f) {
		vec /= scalar;
	}
	return vec;
}

inline vector4_t operator+(float scalar, vector4_t vec) noexcept {
	return vec + scalar;
}

inline vector4_t operator-(float scalar, vector4_t vec) noexcept {
	return { scalar - vec.x, scalar - vec.y, scalar - vec.z, scalar - vec.w };
}

inline vector4_t operator*(float scalar, vector4_t vec) noexcept {
	return vec * scalar;
}

inline vector4_t operator/(float scalar, vector4_t vec) noexcept {
	return { scalar / vec.x, scalar / vec.y, scalar / vec.z, scalar / vec.w };
}

inline std::ostream& operator<<(std::ostream& os, const vector4_t& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	return os;
}
