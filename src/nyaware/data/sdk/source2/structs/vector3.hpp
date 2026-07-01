#pragma once

#include <cmath>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cstdio>

#define M_PI 3.14159265358979323846

struct vector3_t {
	float x{}, y{}, z{};

	constexpr vector3_t() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
	constexpr vector3_t(float value) noexcept : x(value), y(value), z(value) {}
	constexpr vector3_t(float x, float y) noexcept : x(x), y(y), z(0.0f) {}
	constexpr vector3_t(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
	constexpr vector3_t(const float* data) noexcept : x(data[0]), y(data[1]), z(data[2]) {}

	static constexpr vector3_t zero() noexcept { return { 0.0f, 0.0f, 0.0f }; }
	static constexpr vector3_t one() noexcept { return { 1.0f, 1.0f, 1.0f }; }
	static constexpr vector3_t right() noexcept { return { 1.0f, 0.0f, 0.0f }; }
	static constexpr vector3_t left() noexcept { return { -1.0f, 0.0f, 0.0f }; }
	static constexpr vector3_t up() noexcept { return { 0.0f, 1.0f, 0.0f }; }
	static constexpr vector3_t down() noexcept { return { 0.0f, -1.0f, 0.0f }; }
	static constexpr vector3_t forward() noexcept { return { 0.0f, 0.0f, 1.0f }; }
	static constexpr vector3_t back() noexcept { return { 0.0f, 0.0f, -1.0f }; }

	float angle(const vector3_t& other) const noexcept;
	float signed_angle(const vector3_t& to, const vector3_t& axis) const noexcept;

	float pitch(const vector3_t& other) const noexcept;
	float yaw(const vector3_t& other) const noexcept;

	void clamp_magnitude(float max_length) noexcept;
	vector3_t clamped_magnitude(float max_length) const noexcept;

	float component(const vector3_t& b) const noexcept;
	vector3_t cross(const vector3_t& other) const noexcept;
	float distance(const vector3_t& other) const noexcept;
	float dot(const vector3_t& other) const noexcept;

	vector3_t lerp(const vector3_t& other, float t) const noexcept;
	vector3_t lerp_unclamped(const vector3_t& other, float t) const noexcept;

	float magnitude() const noexcept;
	float sqr_magnitude() const noexcept;

	vector3_t move_towards(const vector3_t& target, float max_distance_delta) const noexcept;

	void normalize() noexcept;
	vector3_t normalized() const noexcept;

	vector3_t orthogonal() const noexcept;

	vector3_t project(const vector3_t& onto) const noexcept;
	vector3_t project_on_plane(const vector3_t& plane_normal) const noexcept;
	vector3_t reflect(const vector3_t& plane_normal) const noexcept;
	vector3_t reject(const vector3_t& from) const noexcept;

	vector3_t rotate_towards(const vector3_t& target, float max_radians_delta, float max_magnitude_delta) const noexcept;

	vector3_t scale(const vector3_t& other) const noexcept;

	vector3_t slerp(const vector3_t& other, float t) const noexcept;
	vector3_t slerp_unclamped(const vector3_t& other, float t) const noexcept;

	void set(float new_x, float new_y, float new_z) noexcept;
	void set(float value) noexcept;

	bool is_approximately_zero(float epsilon = 1e-6f) const noexcept;
	bool is_approximately_equal(const vector3_t& other, float epsilon = 1e-6f) const noexcept;

	vector3_t abs() const noexcept;
	vector3_t floor() const noexcept;
	vector3_t ceil() const noexcept;
	vector3_t round() const noexcept;
	vector3_t sign() const noexcept;
	vector3_t clamp_components(float min_val, float max_val) const noexcept;

	std::string to_string() const;

	void to_spherical(float& rad, float& theta, float& phi) const noexcept;

	static float angle(const vector3_t& a, const vector3_t& b) noexcept;
	static float signed_angle(const vector3_t& from, const vector3_t& to, const vector3_t& axis) noexcept;
	static vector3_t clamp_magnitude(const vector3_t& vector, float max_length) noexcept;
	static float component(const vector3_t& a, const vector3_t& b) noexcept;
	static vector3_t cross(const vector3_t& lhs, const vector3_t& rhs) noexcept;
	static float distance(const vector3_t& a, const vector3_t& b) noexcept;
	static float dot(const vector3_t& lhs, const vector3_t& rhs) noexcept;
	static vector3_t lerp(const vector3_t& a, const vector3_t& b, float t) noexcept;
	static vector3_t lerp_unclamped(const vector3_t& a, const vector3_t& b, float t) noexcept;
	static float magnitude(const vector3_t& v) noexcept;
	static vector3_t move_towards(const vector3_t& current, const vector3_t& target, float max_distance_delta) noexcept;
	static vector3_t normalized(const vector3_t& v) noexcept;
	static vector3_t orthogonal(const vector3_t& v) noexcept;
	static void ortho_normalize(vector3_t& normal, vector3_t& tangent, vector3_t& binormal) noexcept;
	static vector3_t project(const vector3_t& a, const vector3_t& b) noexcept;
	static vector3_t project_on_plane(const vector3_t& vector, const vector3_t& plane_normal) noexcept;
	static vector3_t reflect(const vector3_t& vector, const vector3_t& plane_normal) noexcept;
	static vector3_t reject(const vector3_t& a, const vector3_t& b) noexcept;
	static vector3_t rotate_towards(const vector3_t& current, const vector3_t& target, float max_radians_delta, float max_magnitude_delta) noexcept;
	static vector3_t scale(const vector3_t& a, const vector3_t& b) noexcept;
	static vector3_t slerp(const vector3_t& a, const vector3_t& b, float t) noexcept;
	static vector3_t slerp_unclamped(const vector3_t& a, const vector3_t& b, float t) noexcept;
	static float sqr_magnitude(const vector3_t& v) noexcept;
	static std::string to_string(const vector3_t& v);
	static void to_spherical(const vector3_t& vector, float& rad, float& theta, float& phi) noexcept;
	static vector3_t from_spherical(float rad, float theta, float phi) noexcept;

	float& operator[](size_t index) noexcept;
	const float& operator[](size_t index) const noexcept;

	vector3_t operator-() const noexcept;
	vector3_t& operator+=(const vector3_t& other) noexcept;
	vector3_t& operator-=(const vector3_t& other) noexcept;
	vector3_t& operator*=(float scalar) noexcept;
	vector3_t& operator/=(float scalar) noexcept;
	vector3_t& operator*=(const vector3_t& other) noexcept;
	vector3_t& operator/=(const vector3_t& other) noexcept;

	vector3_t& operator=(const vector3_t& other) noexcept = default;

	bool operator==(const vector3_t& other) const noexcept;
	bool operator!=(const vector3_t& other) const noexcept;

	bool equals(const vector3_t& other, float epsilon = 1e-6f) const noexcept;

	friend vector3_t operator+(vector3_t lhs, const vector3_t& rhs) noexcept;
	friend vector3_t operator-(vector3_t lhs, const vector3_t& rhs) noexcept;
	friend vector3_t operator*(vector3_t lhs, const vector3_t& rhs) noexcept;
	friend vector3_t operator/(vector3_t lhs, const vector3_t& rhs) noexcept;

	friend vector3_t operator+(vector3_t vec, float scalar) noexcept;
	friend vector3_t operator-(vector3_t vec, float scalar) noexcept;
	friend vector3_t operator*(vector3_t vec, float scalar) noexcept;
	friend vector3_t operator/(vector3_t vec, float scalar) noexcept;

	friend vector3_t operator+(float scalar, vector3_t vec) noexcept;
	friend vector3_t operator-(float scalar, vector3_t vec) noexcept;
	friend vector3_t operator*(float scalar, vector3_t vec) noexcept;
	friend vector3_t operator/(float scalar, vector3_t vec) noexcept;

	friend std::ostream& operator<<(std::ostream& os, const vector3_t& vec);
};

inline float vector3_t::angle(const vector3_t& other) const noexcept {
	return angle(*this, other);
}

inline float vector3_t::signed_angle(const vector3_t& to, const vector3_t& axis) const noexcept {
	return signed_angle(*this, to, axis);
}

inline float vector3_t::pitch(const vector3_t& other) const noexcept {
	vector3_t delta = other - *this;

	float horizontal_dist = sqrt(delta.x * delta.x + delta.y * delta.y);
	float pitch_rad = atan2(-delta.z, horizontal_dist);

	return pitch_rad * (180.0f / M_PI);
}

inline float vector3_t::yaw(const vector3_t& other) const noexcept {
	vector3_t delta = other - *this;

	float yaw_rad = atan2(delta.y, delta.x);
	return yaw_rad * (180.0f / M_PI);
}

inline void vector3_t::clamp_magnitude(float max_length) noexcept {
	*this = clamp_magnitude(*this, max_length);
}

inline vector3_t vector3_t::clamped_magnitude(float max_length) const noexcept {
	return clamp_magnitude(*this, max_length);
}

inline float vector3_t::component(const vector3_t& b) const noexcept {
	return component(*this, b);
}

inline vector3_t vector3_t::cross(const vector3_t& other) const noexcept {
	return cross(*this, other);
}

inline float vector3_t::distance(const vector3_t& other) const noexcept {
	return distance(*this, other);
}

inline float vector3_t::dot(const vector3_t& other) const noexcept {
	return dot(*this, other);
}

inline vector3_t vector3_t::lerp(const vector3_t& other, float t) const noexcept {
	return lerp(*this, other, t);
}

inline vector3_t vector3_t::lerp_unclamped(const vector3_t& other, float t) const noexcept {
	return lerp_unclamped(*this, other, t);
}

inline float vector3_t::magnitude() const noexcept {
	return magnitude(*this);
}

inline float vector3_t::sqr_magnitude() const noexcept {
	return sqr_magnitude(*this);
}

inline vector3_t vector3_t::move_towards(const vector3_t& target, float max_distance_delta) const noexcept {
	return move_towards(*this, target, max_distance_delta);
}

inline void vector3_t::normalize() noexcept {
	*this = normalized(*this);
}

inline vector3_t vector3_t::normalized() const noexcept {
	return normalized(*this);
}

inline vector3_t vector3_t::orthogonal() const noexcept {
	return orthogonal(*this);
}

inline vector3_t vector3_t::project(const vector3_t& onto) const noexcept {
	return project(*this, onto);
}

inline vector3_t vector3_t::project_on_plane(const vector3_t& plane_normal) const noexcept {
	return project_on_plane(*this, plane_normal);
}

inline vector3_t vector3_t::reflect(const vector3_t& plane_normal) const noexcept {
	return reflect(*this, plane_normal);
}

inline vector3_t vector3_t::reject(const vector3_t& from) const noexcept {
	return reject(*this, from);
}

inline vector3_t vector3_t::rotate_towards(const vector3_t& target, float max_radians_delta, float max_magnitude_delta) const noexcept {
	return rotate_towards(*this, target, max_radians_delta, max_magnitude_delta);
}

inline vector3_t vector3_t::scale(const vector3_t& other) const noexcept {
	return scale(*this, other);
}

inline vector3_t vector3_t::slerp(const vector3_t& other, float t) const noexcept {
	return slerp(*this, other, t);
}

inline vector3_t vector3_t::slerp_unclamped(const vector3_t& other, float t) const noexcept {
	return slerp_unclamped(*this, other, t);
}

inline void vector3_t::set(float new_x, float new_y, float new_z) noexcept {
	x = new_x; y = new_y; z = new_z;
}

inline void vector3_t::set(float value) noexcept {
	x = y = z = value;
}

inline bool vector3_t::is_approximately_zero(float epsilon) const noexcept {
	return sqr_magnitude() < epsilon * epsilon;
}

inline bool vector3_t::is_approximately_equal(const vector3_t& other, float epsilon) const noexcept {
	return distance(*this, other) < epsilon;
}

inline vector3_t vector3_t::abs() const noexcept {
	return { std::abs(x), std::abs(y), std::abs(z) };
}

inline vector3_t vector3_t::floor() const noexcept {
	return { std::floor(x), std::floor(y), std::floor(z) };
}

inline vector3_t vector3_t::ceil() const noexcept {
	return { std::ceil(x), std::ceil(y), std::ceil(z) };
}

inline vector3_t vector3_t::round() const noexcept {
	return { std::round(x), std::round(y), std::round(z) };
}

inline vector3_t vector3_t::sign() const noexcept {
	return {
			(x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f),
			(y > 0.0f) ? 1.0f : ((y < 0.0f) ? -1.0f : 0.0f),
			(z > 0.0f) ? 1.0f : ((z < 0.0f) ? -1.0f : 0.0f)
	};
}

inline vector3_t vector3_t::clamp_components(float min_val, float max_val) const noexcept {
	return {
			std::clamp(x, min_val, max_val),
			std::clamp(y, min_val, max_val),
			std::clamp(z, min_val, max_val)
	};
}

inline std::string vector3_t::to_string() const {
	return to_string(*this);
}

inline void vector3_t::to_spherical(float& rad, float& theta, float& phi) const noexcept {
	to_spherical(*this, rad, theta, phi);
}

inline float vector3_t::angle(const vector3_t& a, const vector3_t& b) noexcept {
	const float dot_val = dot(a, b);
	const float mag_a = magnitude(a);
	const float mag_b = magnitude(b);

	if (mag_a < 1e-6f || mag_b < 1e-6f) return 0.0f;

	const float cos_angle = std::clamp(dot_val / (mag_a * mag_b), -1.0f, 1.0f);
	return std::acos(cos_angle);
}

inline float vector3_t::signed_angle(const vector3_t& from, const vector3_t& to, const vector3_t& axis) noexcept {
	const vector3_t norm_from = normalized(from);
	const vector3_t norm_to = normalized(to);
	const vector3_t norm_axis = normalized(axis);

	const float angle_val = angle(norm_from, norm_to);
	const vector3_t cross_vec = cross(norm_from, norm_to);
	const float sign = (dot(norm_axis, cross_vec) >= 0.0f) ? 1.0f : -1.0f;

	return angle_val * sign;
}

inline vector3_t vector3_t::clamp_magnitude(const vector3_t& vector, float max_length) noexcept {
	const float sqr_mag = sqr_magnitude(vector);
	if (sqr_mag > max_length * max_length) {
		const float mag = std::sqrt(sqr_mag);
		return (vector / mag) * max_length;
	}
	return vector;
}

inline float vector3_t::component(const vector3_t& a, const vector3_t& b) noexcept {
	return dot(a, b) / magnitude(b);
}

inline vector3_t vector3_t::cross(const vector3_t& lhs, const vector3_t& rhs) noexcept {
	return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
	};
}

inline float vector3_t::distance(const vector3_t& a, const vector3_t& b) noexcept {
	return magnitude(a - b);
}

inline float vector3_t::dot(const vector3_t& lhs, const vector3_t& rhs) noexcept {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline vector3_t vector3_t::lerp(const vector3_t& a, const vector3_t& b, float t) noexcept {
	t = std::clamp(t, 0.0f, 1.0f);
	return lerp_unclamped(a, b, t);
}

inline vector3_t vector3_t::lerp_unclamped(const vector3_t& a, const vector3_t& b, float t) noexcept {
	return {
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t
	};
}

inline float vector3_t::magnitude(const vector3_t& v) noexcept {
	return std::sqrt(sqr_magnitude(v));
}

inline vector3_t vector3_t::move_towards(const vector3_t& current, const vector3_t& target, float max_distance_delta) noexcept {
	const vector3_t delta = target - current;
	const float distance = magnitude(delta);

	if (distance <= max_distance_delta || distance < 1e-6f) {
		return target;
	}

	return current + (delta / distance) * max_distance_delta;
}

inline vector3_t vector3_t::normalized(const vector3_t& v) noexcept {
	const float mag = magnitude(v);
	return (mag > 1e-6f) ? (v / mag) : zero();
}

inline vector3_t vector3_t::orthogonal(const vector3_t& v) noexcept {
	return (std::abs(v.z) < std::abs(v.x)) ? vector3_t(v.y, -v.x, 0.0f) : vector3_t(0.0f, -v.z, v.y);
}

inline void vector3_t::ortho_normalize(vector3_t& normal, vector3_t& tangent, vector3_t& binormal) noexcept {
	normal = normalized(normal);
	tangent = project_on_plane(tangent, normal);
	tangent = normalized(tangent);
	binormal = project_on_plane(binormal, tangent);
	binormal = project_on_plane(binormal, normal);
	binormal = normalized(binormal);
}

inline vector3_t vector3_t::project(const vector3_t& a, const vector3_t& b) noexcept {
	const float sqr_mag = sqr_magnitude(b);
	if (sqr_mag < 1e-6f) return zero();
	return b * (dot(a, b) / sqr_mag);
}

inline vector3_t vector3_t::project_on_plane(const vector3_t& vector, const vector3_t& plane_normal) noexcept {
	return vector - project(vector, plane_normal);
}

inline vector3_t vector3_t::reflect(const vector3_t& vector, const vector3_t& plane_normal) noexcept {
	return vector - 2.0f * project(vector, plane_normal);
}

inline vector3_t vector3_t::reject(const vector3_t& a, const vector3_t& b) noexcept {
	return a - project(a, b);
}

inline vector3_t vector3_t::rotate_towards(const vector3_t& current, const vector3_t& target, float max_radians_delta, float max_magnitude_delta) noexcept {
	const float mag_cur = magnitude(current);
	const float mag_tar = magnitude(target);
	const float mag_delta = (mag_tar > mag_cur) ? max_magnitude_delta : -max_magnitude_delta;

	float new_mag{};
	if (std::abs(mag_tar - mag_cur) <= max_magnitude_delta) {
		new_mag = mag_tar;
	}
	else {
		new_mag = mag_cur + ((mag_tar > mag_cur) ? 1.0f : -1.0f) * max_magnitude_delta;
	}

	const float total_angle = angle(current, target);

	if (total_angle <= max_radians_delta) {
		return normalized(target) * new_mag;
	}

	if (total_angle >= M_PI - max_radians_delta) {
		return normalized(-target) * new_mag;
	}

	const vector3_t axis = cross(current, target);
	const vector3_t norm_axis = (magnitude(axis) < 1e-6f) ? orthogonal(current) : normalized(axis);

	const vector3_t norm_current = normalized(current);
	const float cos_theta = std::cos(max_radians_delta);
	const float sin_theta = std::sin(max_radians_delta);

	return (norm_current * cos_theta + cross(norm_axis, norm_current) * sin_theta + norm_axis * dot(norm_axis, norm_current) * (1.0f - cos_theta)) * new_mag;
}

inline vector3_t vector3_t::scale(const vector3_t& a, const vector3_t& b) noexcept {
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}

inline vector3_t vector3_t::slerp(const vector3_t& a, const vector3_t& b, float t) noexcept {
	t = std::clamp(t, 0.0f, 1.0f);
	return slerp_unclamped(a, b, t);
}

inline vector3_t vector3_t::slerp_unclamped(const vector3_t& a, const vector3_t& b, float t) noexcept {
	const float mag_a = magnitude(a);
	const float mag_b = magnitude(b);

	if (mag_a < 1e-6f || mag_b < 1e-6f) {
		return lerp_unclamped(a, b, t);
	}

	const vector3_t norm_a = a / mag_a;
	const vector3_t norm_b = b / mag_b;

	float dot_val = dot(norm_a, norm_b);
	dot_val = std::clamp(dot_val, -1.0f, 1.0f);

	const float theta = std::acos(dot_val) * t;
	const vector3_t relative_vec = normalized(norm_b - norm_a * dot_val);

	const float cos_theta = std::cos(theta);
	const float sin_theta = std::sin(theta);

	const vector3_t new_vec = norm_a * cos_theta + relative_vec * sin_theta;
	return new_vec * (mag_a + (mag_b - mag_a) * t);
}

inline float vector3_t::sqr_magnitude(const vector3_t& v) noexcept {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline std::string vector3_t::to_string(const vector3_t& v) {
	char buffer[64];
	std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f)", v.x, v.y, v.z);
	return std::string(buffer);
}

inline void vector3_t::to_spherical(const vector3_t& vector, float& rad, float& theta, float& phi) noexcept {
	rad = magnitude(vector);
	if (rad < 1e-6f) {
		theta = 0.0f;
		phi = 0.0f;
		return;
	}

	theta = std::acos(std::clamp(vector.z / rad, -1.0f, 1.0f));
	phi = std::atan2(vector.y, vector.x);
}

inline vector3_t vector3_t::from_spherical(float rad, float theta, float phi) noexcept {
	const float sin_theta = std::sin(theta);
	return {
			rad * sin_theta * std::cos(phi),
			rad * sin_theta * std::sin(phi),
			rad * std::cos(theta)
	};
}

inline vector3_t vector3_t::operator-() const noexcept {
	return { -x, -y, -z };
}

inline vector3_t& vector3_t::operator+=(const vector3_t& other) noexcept {
	x += other.x; y += other.y; z += other.z;
	return *this;
}

inline vector3_t& vector3_t::operator-=(const vector3_t& other) noexcept {
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}

inline vector3_t& vector3_t::operator*=(float scalar) noexcept {
	x *= scalar; y *= scalar; z *= scalar;
	return *this;
}

inline vector3_t& vector3_t::operator/=(float scalar) noexcept {
	const float inv_scalar = 1.0f / scalar;
	x *= inv_scalar; y *= inv_scalar; z *= inv_scalar;
	return *this;
}

inline vector3_t& vector3_t::operator*=(const vector3_t& other) noexcept {
	x *= other.x; y *= other.y; z *= other.z;
	return *this;
}

inline vector3_t& vector3_t::operator/=(const vector3_t& other) noexcept {
	x /= other.x; y /= other.y; z /= other.z;
	return *this;
}

inline bool vector3_t::operator==(const vector3_t& other) const noexcept {
	return x == other.x && y == other.y && z == other.z;
}

inline bool vector3_t::operator!=(const vector3_t& other) const noexcept {
	return !(*this == other);
}

inline bool vector3_t::equals(const vector3_t& other, float epsilon) const noexcept {
	return distance(*this, other) < epsilon;
}

inline vector3_t operator+(vector3_t lhs, const vector3_t& rhs) noexcept {
	lhs += rhs;
	return lhs;
}

inline vector3_t operator-(vector3_t lhs, const vector3_t& rhs) noexcept {
	lhs -= rhs;
	return lhs;
}

inline vector3_t operator*(vector3_t lhs, const vector3_t& rhs) noexcept {
	lhs *= rhs;
	return lhs;
}

inline vector3_t operator/(vector3_t lhs, const vector3_t& rhs) noexcept {
	lhs /= rhs;
	return lhs;
}

inline vector3_t operator+(vector3_t vec, float scalar) noexcept {
	return { vec.x + scalar, vec.y + scalar, vec.z + scalar };
}

inline vector3_t operator-(vector3_t vec, float scalar) noexcept {
	return { vec.x - scalar, vec.y - scalar, vec.z - scalar };
}

inline vector3_t operator*(vector3_t vec, float scalar) noexcept {
	vec *= scalar;
	return vec;
}

inline vector3_t operator/(vector3_t vec, float scalar) noexcept {
	vec /= scalar;
	return vec;
}

inline vector3_t operator+(float scalar, vector3_t vec) noexcept {
	return vec + scalar;
}

inline vector3_t operator-(float scalar, vector3_t vec) noexcept {
	return { scalar - vec.x, scalar - vec.y, scalar - vec.z };
}

inline vector3_t operator*(float scalar, vector3_t vec) noexcept {
	return vec * scalar;
}

inline vector3_t operator/(float scalar, vector3_t vec) noexcept {
	return { scalar / vec.x, scalar / vec.y, scalar / vec.z };
}

inline std::ostream& operator<<(std::ostream& os, const vector3_t& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}