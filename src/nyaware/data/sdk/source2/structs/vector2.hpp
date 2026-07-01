#pragma once

#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <ostream>
#include <limits>
#include <cstdio>

#define M_PI 3.14159265358979323846

struct vector2_t {
	float x{}, y{};

	constexpr vector2_t() noexcept : x(0.0f), y(0.0f) {}
	constexpr vector2_t(float x, float y) noexcept : x(x), y(y) {}
	constexpr vector2_t(float value) noexcept : x(value), y(value) {}
	constexpr vector2_t(const float* data) noexcept : x(data[0]), y(data[1]) {}

	static constexpr vector2_t zero() noexcept { return { 0.0f, 0.0f }; }
	static constexpr vector2_t one() noexcept { return { 1.0f, 1.0f }; }
	static constexpr vector2_t right() noexcept { return { 1.0f, 0.0f }; }
	static constexpr vector2_t left() noexcept { return { -1.0f, 0.0f }; }
	static constexpr vector2_t up() noexcept { return { 0.0f, 1.0f }; }
	static constexpr vector2_t down() noexcept { return { 0.0f, -1.0f }; }

	float angle(const vector2_t& other) const noexcept;
	float signed_angle(const vector2_t& to) const noexcept;

	void clamp_magnitude(float max_length) noexcept;
	vector2_t clamped_magnitude(float max_length) const noexcept;

	float component(const vector2_t& b) const noexcept;
	float distance(const vector2_t& other) const noexcept;
	float dot(const vector2_t& other) const noexcept;
	float cross(const vector2_t& other) const noexcept;

	vector2_t lerp(const vector2_t& other, float t) const noexcept;
	vector2_t lerp_unclamped(const vector2_t& other, float t) const noexcept;

	float magnitude() const noexcept;
	float sqr_magnitude() const noexcept;

	vector2_t move_towards(const vector2_t& target, float max_distance_delta) const noexcept;

	void normalize() noexcept;
	vector2_t normalized() const noexcept;

	vector2_t project(const vector2_t& onto) const noexcept;
	vector2_t reflect(const vector2_t& normal) const noexcept;
	vector2_t reject(const vector2_t& from) const noexcept;

	vector2_t rotate_towards(const vector2_t& target, float max_radians_delta, float max_magnitude_delta) const noexcept;
	vector2_t rotated(float angle_rad) const noexcept;
	vector2_t rotated_degrees(float angle_deg) const noexcept;

	vector2_t scale(const vector2_t& other) const noexcept;

	vector2_t slerp(const vector2_t& other, float t) const noexcept;
	vector2_t slerp_unclamped(const vector2_t& other, float t) const noexcept;

	void to_polar(float& rad, float& theta) const noexcept;

	std::string to_string() const;

	vector2_t perpendicular() const noexcept;
	vector2_t perpendicular_left() const noexcept;

	bool is_approximately_zero(float epsilon = 1e-6f) const noexcept;
	bool is_approximately_equal(const vector2_t& other, float epsilon = 1e-6f) const noexcept;

	vector2_t abs() const noexcept;
	vector2_t floor() const noexcept;
	vector2_t ceil() const noexcept;
	vector2_t round() const noexcept;
	vector2_t sign() const noexcept;
	vector2_t clamp_components(float min_val, float max_val) const noexcept;

	void set(float new_x, float new_y) noexcept;
	void set(float value) noexcept;

	float angle() const noexcept;
	float angle_degrees() const noexcept;

	static float angle(const vector2_t& a, const vector2_t& b) noexcept;
	static float signed_angle(const vector2_t& from, const vector2_t& to) noexcept;
	static vector2_t clamp_magnitude(const vector2_t& vector, float max_length) noexcept;
	static float component(const vector2_t& a, const vector2_t& b) noexcept;
	static float distance(const vector2_t& a, const vector2_t& b) noexcept;
	static float dot(const vector2_t& lhs, const vector2_t& rhs) noexcept;
	static float cross(const vector2_t& lhs, const vector2_t& rhs) noexcept;
	static vector2_t lerp(const vector2_t& a, const vector2_t& b, float t) noexcept;
	static vector2_t lerp_unclamped(const vector2_t& a, const vector2_t& b, float t) noexcept;
	static float magnitude(const vector2_t& v) noexcept;
	static float sqr_magnitude(const vector2_t& v) noexcept;
	static vector2_t move_towards(const vector2_t& current, const vector2_t& target, float max_distance_delta) noexcept;
	static vector2_t normalized(const vector2_t& v) noexcept;
	static void ortho_normalize(vector2_t& normal, vector2_t& tangent) noexcept;
	static vector2_t project(const vector2_t& a, const vector2_t& b) noexcept;
	static vector2_t reflect(const vector2_t& vector, const vector2_t& normal) noexcept;
	static vector2_t reject(const vector2_t& a, const vector2_t& b) noexcept;
	static vector2_t rotate_towards(const vector2_t& current, const vector2_t& target, float max_radians_delta, float max_magnitude_delta) noexcept;
	static vector2_t scale(const vector2_t& a, const vector2_t& b) noexcept;
	static vector2_t slerp(const vector2_t& a, const vector2_t& b, float t) noexcept;
	static vector2_t slerp_unclamped(const vector2_t& a, const vector2_t& b, float t) noexcept;
	static void to_polar(const vector2_t& vector, float& rad, float& theta) noexcept;
	static vector2_t from_polar(float rad, float theta) noexcept;
	static vector2_t rotate(const vector2_t& vector, float angle_rad) noexcept;
	static std::string to_string(const vector2_t& v);

	float& operator[](size_t index) noexcept;
	const float& operator[](size_t index) const noexcept;

	vector2_t operator-() const noexcept;
	vector2_t& operator+=(const vector2_t& other) noexcept;
	vector2_t& operator-=(const vector2_t& other) noexcept;
	vector2_t& operator*=(float scalar) noexcept;
	vector2_t& operator/=(float scalar) noexcept;
	vector2_t& operator*=(const vector2_t& other) noexcept;
	vector2_t& operator/=(const vector2_t& other) noexcept;

	vector2_t& operator=(const vector2_t& other) noexcept = default;

	bool operator==(const vector2_t& other) const noexcept;
	bool operator!=(const vector2_t& other) const noexcept;
	bool equals(const vector2_t& other, float epsilon = 1e-6f) const noexcept;

	friend vector2_t operator+(vector2_t lhs, const vector2_t& rhs) noexcept;
	friend vector2_t operator-(vector2_t lhs, const vector2_t& rhs) noexcept;
	friend vector2_t operator*(vector2_t lhs, const vector2_t& rhs) noexcept;
	friend vector2_t operator/(vector2_t lhs, const vector2_t& rhs) noexcept;

	friend vector2_t operator+(vector2_t vec, float scalar) noexcept;
	friend vector2_t operator-(vector2_t vec, float scalar) noexcept;
	friend vector2_t operator*(vector2_t vec, float scalar) noexcept;
	friend vector2_t operator/(vector2_t vec, float scalar) noexcept;

	friend vector2_t operator+(float scalar, const vector2_t& vec) noexcept;
	friend vector2_t operator-(float scalar, const vector2_t& vec) noexcept;
	friend vector2_t operator*(float scalar, const vector2_t& vec) noexcept;
	friend vector2_t operator/(float scalar, const vector2_t& vec) noexcept;

	friend std::ostream& operator<<(std::ostream& os, const vector2_t& vec);
};

inline float vector2_t::angle(const vector2_t& other) const noexcept {
	return angle(*this, other);
}

inline float vector2_t::signed_angle(const vector2_t& to) const noexcept {
	return vector2_t::signed_angle(*this, to);
}

inline void vector2_t::clamp_magnitude(float max_length) noexcept {
	*this = clamp_magnitude(*this, max_length);
}

inline vector2_t vector2_t::clamped_magnitude(float max_length) const noexcept {
	return clamp_magnitude(*this, max_length);
}

inline float vector2_t::component(const vector2_t& b) const noexcept {
	return component(*this, b);
}

inline float vector2_t::distance(const vector2_t& other) const noexcept {
	return distance(*this, other);
}

inline float vector2_t::dot(const vector2_t& other) const noexcept {
	return dot(*this, other);
}

inline float vector2_t::cross(const vector2_t& other) const noexcept {
	return cross(*this, other);
}

inline vector2_t vector2_t::lerp(const vector2_t& other, float t) const noexcept {
	return lerp(*this, other, t);
}

inline vector2_t vector2_t::lerp_unclamped(const vector2_t& other, float t) const noexcept {
	return lerp_unclamped(*this, other, t);
}

inline float vector2_t::magnitude() const noexcept {
	return magnitude(*this);
}

inline float vector2_t::sqr_magnitude() const noexcept {
	return sqr_magnitude(*this);
}

inline vector2_t vector2_t::move_towards(const vector2_t& target, float max_distance_delta) const noexcept {
	return move_towards(*this, target, max_distance_delta);
}

inline void vector2_t::normalize() noexcept {
	const float mag = magnitude();
	if (mag > 1e-6f) {
		x /= mag;
		y /= mag;
	}
	else {
		*this = zero();
	}
}

inline vector2_t vector2_t::normalized() const noexcept {
	return normalized(*this);
}

inline vector2_t vector2_t::project(const vector2_t& onto) const noexcept {
	return project(*this, onto);
}

inline vector2_t vector2_t::reflect(const vector2_t& normal) const noexcept {
	return reflect(*this, normal);
}

inline vector2_t vector2_t::reject(const vector2_t& from) const noexcept {
	return reject(*this, from);
}

inline vector2_t vector2_t::rotate_towards(const vector2_t& target, float max_radians_delta, float max_magnitude_delta) const noexcept {
	return rotate_towards(*this, target, max_radians_delta, max_magnitude_delta);
}

inline vector2_t vector2_t::rotated(float angle_rad) const noexcept {
	const float cos_a = std::cos(angle_rad);
	const float sin_a = std::sin(angle_rad);
	return {
			x * cos_a - y * sin_a,
			x * sin_a + y * cos_a
	};
}

inline vector2_t vector2_t::rotated_degrees(float angle_deg) const noexcept {
	return rotated(angle_deg * (M_PI / 180.0f));
}

inline vector2_t vector2_t::scale(const vector2_t& other) const noexcept {
	return scale(*this, other);
}

inline vector2_t vector2_t::slerp(const vector2_t& other, float t) const noexcept {
	return slerp(*this, other, t);
}

inline vector2_t vector2_t::slerp_unclamped(const vector2_t& other, float t) const noexcept {
	return slerp_unclamped(*this, other, t);
}

inline void vector2_t::to_polar(float& rad, float& theta) const noexcept {
	to_polar(*this, rad, theta);
}

inline std::string vector2_t::to_string() const {
	return to_string(*this);
}

inline vector2_t vector2_t::perpendicular() const noexcept {
	return { -y, x };
}

inline vector2_t vector2_t::perpendicular_left() const noexcept {
	return { y, -x };
}

inline bool vector2_t::is_approximately_zero(float epsilon) const noexcept {
	return sqr_magnitude() < epsilon * epsilon;
}

inline bool vector2_t::is_approximately_equal(const vector2_t& other, float epsilon) const noexcept {
	return distance(other) < epsilon;
}

inline vector2_t vector2_t::abs() const noexcept {
	return { std::abs(x), std::abs(y) };
}

inline vector2_t vector2_t::floor() const noexcept {
	return { std::floor(x), std::floor(y) };
}

inline vector2_t vector2_t::ceil() const noexcept {
	return { std::ceil(x), std::ceil(y) };
}

inline vector2_t vector2_t::round() const noexcept {
	return { std::round(x), std::round(y) };
}

inline vector2_t vector2_t::sign() const noexcept {
	return {
			(x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f),
			(y > 0.0f) ? 1.0f : ((y < 0.0f) ? -1.0f : 0.0f)
	};
}

inline vector2_t vector2_t::clamp_components(float min_val, float max_val) const noexcept {
	return {
			std::clamp(x, min_val, max_val),
			std::clamp(y, min_val, max_val)
	};
}

inline void vector2_t::set(float new_x, float new_y) noexcept {
	x = new_x; y = new_y;
}

inline void vector2_t::set(float value) noexcept {
	x = y = value;
}

inline float vector2_t::angle() const noexcept {
	return std::atan2(y, x);
}

inline float vector2_t::angle_degrees() const noexcept {
	return angle() * (180.0f / M_PI);
}

inline float vector2_t::angle(const vector2_t& a, const vector2_t& b) noexcept {
	const float dot_val = dot(a, b);
	const float mag_a = magnitude(a);
	const float mag_b = magnitude(b);

	if (mag_a < 1e-6f || mag_b < 1e-6f) return 0.0f;

	const float cos_angle = std::clamp(dot_val / (mag_a * mag_b), -1.0f, 1.0f);
	return std::acos(cos_angle);
}

inline float vector2_t::signed_angle(const vector2_t& from, const vector2_t& to) noexcept {
	return std::atan2(cross(from, to), dot(from, to));
}

inline vector2_t vector2_t::clamp_magnitude(const vector2_t& vector, float max_length) noexcept {
	const float sqr_mag = sqr_magnitude(vector);
	if (sqr_mag > max_length * max_length) {
		const float mag = std::sqrt(sqr_mag);
		return vector2_t(vector.x / mag, vector.y / mag) * max_length;
	}

	return vector;
}

inline float vector2_t::component(const vector2_t& a, const vector2_t& b) noexcept {
	return dot(a, b) / magnitude(b);
}

inline float vector2_t::distance(const vector2_t& a, const vector2_t& b) noexcept {
	return magnitude(vector2_t(a.x - b.x, a.y - b.y));
}

inline float vector2_t::dot(const vector2_t& lhs, const vector2_t& rhs) noexcept {
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline float vector2_t::cross(const vector2_t& lhs, const vector2_t& rhs) noexcept {
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

inline vector2_t vector2_t::lerp(const vector2_t& a, const vector2_t& b, float t) noexcept {
	t = std::clamp(t, 0.0f, 1.0f);
	return lerp_unclamped(a, b, t);
}

inline vector2_t vector2_t::lerp_unclamped(const vector2_t& a, const vector2_t& b, float t) noexcept {
	return {
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t
	};
}

inline float vector2_t::magnitude(const vector2_t& v) noexcept {
	return std::sqrt(sqr_magnitude(v));
}

inline float vector2_t::sqr_magnitude(const vector2_t& v) noexcept {
	return v.x * v.x + v.y * v.y;
}

inline vector2_t vector2_t::move_towards(const vector2_t& current, const vector2_t& target, float max_distance_delta) noexcept {
	const vector2_t delta(target.x - current.x, target.y - current.y);
	const float distance = magnitude(delta);

	if (distance <= max_distance_delta || distance < 1e-6f) {
		return target;
	}

	return vector2_t(current.x + (delta.x / distance) * max_distance_delta, current.y + (delta.y / distance) * max_distance_delta);
}

inline vector2_t vector2_t::normalized(const vector2_t& v) noexcept {
	const float mag = magnitude(v);
	if (mag > 1e-6f) {
		return { v.x / mag, v.y / mag };
	}

	return zero();
}

inline void vector2_t::ortho_normalize(vector2_t& normal, vector2_t& tangent) noexcept {
	normal = normalized(normal);
	tangent = reject(tangent, normal);
	tangent = normalized(tangent);
}

inline vector2_t vector2_t::project(const vector2_t& a, const vector2_t& b) noexcept {
	const float sqr_mag = sqr_magnitude(b);
	if (sqr_mag < 1e-6f) return zero();
	return b * (dot(a, b) / sqr_mag);
}

inline vector2_t vector2_t::reflect(const vector2_t& vector, const vector2_t& normal) noexcept {
	return vector - normal * (2.0f * dot(vector, normal));
}

inline vector2_t vector2_t::reject(const vector2_t& a, const vector2_t& b) noexcept {
	return a - project(a, b);
}

inline vector2_t vector2_t::rotate_towards(const vector2_t& current, const vector2_t& target, float max_radians_delta, float max_magnitude_delta) noexcept {
	const float mag_cur = magnitude(current);
	const float mag_tar = magnitude(target);

	float new_mag{};
	if (std::abs(mag_tar - mag_cur) <= max_magnitude_delta) {
		new_mag = mag_tar;
	}
	else {
		new_mag = mag_cur + ((mag_tar > mag_cur) ? 1.0f : -1.0f) * max_magnitude_delta;
	}

	const float total_angle = angle(current, target);
	if (total_angle <= max_radians_delta || total_angle < 1e-6f) {
		return normalized(target) * new_mag;
	}

	const float axis_sign = (cross(current, target) >= 0.0f) ? 1.0f : -1.0f;
	const vector2_t norm_current = normalized(current);
	const float cos_theta = std::cos(max_radians_delta);
	const float sin_theta = std::sin(max_radians_delta);

	const vector2_t rotated_vec = {
			norm_current.x * cos_theta - norm_current.y * sin_theta * axis_sign,
			norm_current.x * sin_theta * axis_sign + norm_current.y * cos_theta
	};

	return rotated_vec * new_mag;
}

inline vector2_t vector2_t::scale(const vector2_t& a, const vector2_t& b) noexcept {
	return { a.x * b.x, a.y * b.y };
}

inline vector2_t vector2_t::slerp(const vector2_t& a, const vector2_t& b, float t) noexcept {
	t = std::clamp(t, 0.0f, 1.0f);
	return slerp_unclamped(a, b, t);
}

inline vector2_t vector2_t::slerp_unclamped(const vector2_t& a, const vector2_t& b, float t) noexcept {
	const float mag_a = magnitude(a);
	const float mag_b = magnitude(b);

	if (mag_a < 1e-6f || mag_b < 1e-6f) {
		return lerp_unclamped(a, b, t);
	}

	const vector2_t norm_a = a / mag_a;
	const vector2_t norm_b = b / mag_b;

	float dot_val = dot(norm_a, norm_b);
	dot_val = std::clamp(dot_val, -1.0f, 1.0f);

	const float theta = std::acos(dot_val) * t;
	const vector2_t relative_vec = normalized(norm_b - norm_a * dot_val);

	const float cos_theta = std::cos(theta);
	const float sin_theta = std::sin(theta);

	const vector2_t new_vec = norm_a * cos_theta + relative_vec * sin_theta;
	return new_vec * (mag_a + (mag_b - mag_a) * t);
}

inline void vector2_t::to_polar(const vector2_t& vector, float& rad, float& theta) noexcept {
	rad = magnitude(vector);
	theta = std::atan2(vector.y, vector.x);
}

inline vector2_t vector2_t::from_polar(float rad, float theta) noexcept {
	return { rad * std::cos(theta), rad * std::sin(theta) };
}

inline vector2_t vector2_t::rotate(const vector2_t& vector, float angle_rad) noexcept {
	const float cos_a = std::cos(angle_rad);
	const float sin_a = std::sin(angle_rad);
	return { vector.x * cos_a - vector.y * sin_a, vector.x * sin_a + vector.y * cos_a };
}

inline std::string vector2_t::to_string(const vector2_t& v) {
	char buffer[48];
	std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f)", v.x, v.y);
	return std::string(buffer);
}

inline vector2_t vector2_t::operator-() const noexcept {
	return { -x, -y };
}

inline vector2_t& vector2_t::operator+=(const vector2_t& other) noexcept {
	x += other.x; y += other.y;
	return *this;
}

inline vector2_t& vector2_t::operator-=(const vector2_t& other) noexcept {
	x -= other.x; y -= other.y;
	return *this;
}

inline vector2_t& vector2_t::operator*=(float scalar) noexcept {
	x *= scalar; y *= scalar;
	return *this;
}

inline vector2_t& vector2_t::operator/=(float scalar) noexcept {
	const float inv_scalar = 1.0f / scalar;
	x *= inv_scalar; y *= inv_scalar;
	return *this;
}

inline vector2_t& vector2_t::operator*=(const vector2_t& other) noexcept {
	x *= other.x; y *= other.y;
	return *this;
}

inline vector2_t& vector2_t::operator/=(const vector2_t& other) noexcept {
	x /= other.x; y /= other.y;
	return *this;
}

inline bool vector2_t::operator==(const vector2_t& other) const noexcept {
	return x == other.x && y == other.y;
}

inline bool vector2_t::operator!=(const vector2_t& other) const noexcept {
	return !(*this == other);
}

inline bool vector2_t::equals(const vector2_t& other, float epsilon) const noexcept {
	return std::abs(x - other.x) < epsilon && std::abs(y - other.y) < epsilon;
}

inline vector2_t operator+(vector2_t lhs, const vector2_t& rhs) noexcept {
	lhs += rhs;
	return lhs;
}

inline vector2_t operator-(vector2_t lhs, const vector2_t& rhs) noexcept {
	lhs -= rhs;
	return lhs;
}

inline vector2_t operator*(vector2_t lhs, const vector2_t& rhs) noexcept {
	lhs *= rhs;
	return lhs;
}

inline vector2_t operator/(vector2_t lhs, const vector2_t& rhs) noexcept {
	lhs /= rhs;
	return lhs;
}

inline vector2_t operator+(vector2_t vec, float scalar) noexcept {
	vec.x += scalar; vec.y += scalar;
	return vec;
}

inline vector2_t operator-(vector2_t vec, float scalar) noexcept {
	vec.x -= scalar; vec.y -= scalar;
	return vec;
}

inline vector2_t operator*(vector2_t vec, float scalar) noexcept {
	vec *= scalar;
	return vec;
}

inline vector2_t operator/(vector2_t vec, float scalar) noexcept {
	vec /= scalar;
	return vec;
}

inline vector2_t operator+(float scalar, const vector2_t& vec) noexcept {
	return vector2_t(scalar + vec.x, scalar + vec.y);
}

inline vector2_t operator-(float scalar, const vector2_t& vec) noexcept {
	return vector2_t(scalar - vec.x, scalar - vec.y);
}

inline vector2_t operator*(float scalar, const vector2_t& vec) noexcept {
	return vector2_t(scalar * vec.x, scalar * vec.y);
}

inline vector2_t operator/(float scalar, const vector2_t& vec) noexcept {
	return vector2_t(scalar / vec.x, scalar / vec.y);
}

inline std::ostream& operator<<(std::ostream& os, const vector2_t& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}