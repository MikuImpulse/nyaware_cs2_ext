#pragma once

#include "vector3.hpp"
#include "vector4.hpp"

#include "interface/interface.hpp"

struct matrix_t {
	float m[4][4]{};

	constexpr matrix_t() noexcept {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
	}

	constexpr float* operator[](int index) noexcept { return m[index]; }
	constexpr const float* operator[](int index) const noexcept { return m[index]; }

	constexpr matrix_t operator+(const matrix_t& other) const noexcept {
		matrix_t result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[i][j] + other.m[i][j];
			}
		}
		return result;
	}

	constexpr matrix_t operator-(const matrix_t& other) const noexcept {
		matrix_t result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[i][j] - other.m[i][j];
			}
		}
		return result;
	}

	constexpr matrix_t operator*(const matrix_t& other) const noexcept {
		matrix_t result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = 0.0f;
				for (int k = 0; k < 4; ++k) {
					result.m[i][j] += m[i][k] * other.m[k][j];
				}
			}
		}
		return result;
	}

	constexpr matrix_t operator*(float scalar) const noexcept {
		matrix_t result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[i][j] * scalar;
			}
		}
		return result;
	}

	constexpr matrix_t operator/(float scalar) const noexcept {
		const float inv = 1.0f / scalar;
		return *this * inv;
	}

	constexpr vector4_t operator*(const vector4_t& vec) const noexcept {
		return {
				m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.w,
				m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.w,
				m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.w,
				m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.w
		};
	}

	constexpr vector3_t multiply_vector(const vector3_t& vector) const noexcept {
		return {
				m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z,
				m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z,
				m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z
		};
	}

	constexpr matrix_t transposed() const noexcept {
		matrix_t result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[j][i];
			}
		}
		return result;
	}

	void transpose() noexcept {
		*this = transposed();
	}

	static constexpr matrix_t translation(const vector3_t& translation) noexcept {
		matrix_t mat;
		mat.m[0][3] = translation.x;
		mat.m[1][3] = translation.y;
		mat.m[2][3] = translation.z;
		return mat;
	}

	static constexpr matrix_t scale(const vector3_t& scale) noexcept {
		matrix_t mat;
		mat.m[0][0] = scale.x;
		mat.m[1][1] = scale.y;
		mat.m[2][2] = scale.z;
		return mat;
	}

	static constexpr vector3_t get_translation(const matrix_t& matrix) noexcept {
		return { matrix.m[0][3], matrix.m[1][3], matrix.m[2][3] };
	}

	inline constexpr vector3_t worldToScreenPoint(const screen_t& screen, vector3_t position) const noexcept {
		vector3_t result = vector3_t::zero();

		vector4_t clipPoint = *this * vector4_t(position.x, position.y, position.z, 1);
		if (clipPoint.w != 0 || clipPoint.z > 0) {
			result.x = (clipPoint.x / clipPoint.w + 1) * .5f * screen.width;
			result.y = screen.height - (clipPoint.y / clipPoint.w + 1) * .5f * screen.height;
			result.z = clipPoint.z;
		}

		return result;
	}
};