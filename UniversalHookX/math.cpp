#include "math.hpp"
#include <math.h>
#include <cstdio>

namespace Math {

    // Vector4 Implementation
    Vector4& Vector4::operator+=(const Vector4& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        this->w += rhs.w;
        return *this;
    }
    Vector4& Vector4::operator-=(const Vector4& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        this->w -= rhs.w;
        return *this;
    }
    Vector4 Vector4::operator+(const Vector4& rhs) {
        Vector4 result = *this;
        result += rhs;
        return result;
    }
    Vector4 Vector4::operator-(const Vector4& rhs) {
        Vector4 result = *this;
        result -= rhs;
        return result;
    }
    Vector4 Vector4::operator*(const float scalar) {
        Vector4 result = *this;
        result.x *= scalar;
        result.y *= scalar;
        result.z *= scalar;
        result.w *= scalar;
        return result;
    }
    Vector4 Vector4::operator/(const float scalar) {
        float inv = 1.0f / scalar;
        Vector4 result = *this;
        result.x *= inv;
        result.y *= inv;
        result.z *= inv;
        result.w *= inv;
        return result;
    }
    float Vector4::dot(const Vector4& rhs) {
        return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w * rhs.w;
    }
    float Vector4::length() {
        return sqrtf(this->dot(*this));
    }
    Vector4 Vector4::cross(const Vector4& rhs) {
        Vector4 result;
        result.x = this->y * rhs.z - this->z * rhs.y;
        result.y = this->z * rhs.x - this->x * rhs.z;
        result.z = this->x * rhs.y - this->y * rhs.x;
        result.w = 0;
        return result;
    }

    // Matrix4 Implementation
    Matrix4 Matrix4::operator*(const Matrix4& rhs) {
        Matrix4 result;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                result.m[c][r] = 0;
                for (int i = 0; i < 4; i++) {
                    result.m[c][r] += this->m[i][r] * rhs.m[c][i];
                }
            }
        }
        return result;
    }

    Vector4 Matrix4::operator*(const Vector4& rhs) {
        Vector4 result;
        float* fResult = (float*)&result;
        float* fRhs = (float*)&rhs;
        for (int r = 0; r < 4; r++) {
            fResult[r] = 0;
            for (int i = 0; i < 4; i++) {
                fResult[r] += this->m[i][r] * fRhs[i];
            }
        }
        return result;
    }

    Matrix4 Matrix4::transpose() {
        Matrix4 result;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                result.m[r][c] = this->m[c][r];
        return result;
    }

    Matrix4 Matrix4::inverse(bool& success) {
        Matrix4 result{0};

        float m00 = this->m[0][0];
        float m01 = this->m[0][1];
        float m02 = this->m[0][2];
        float m03 = this->m[0][3];
        
        float m10 = this->m[1][0];
        float m11 = this->m[1][1];
        float m12 = this->m[1][2];
        float m13 = this->m[1][3];

        float m20 = this->m[2][0];
        float m21 = this->m[2][1];
        float m22 = this->m[2][2];
        float m23 = this->m[2][3];

        float m30 = this->m[3][0];
        float m31 = this->m[3][1];
        float m32 = this->m[3][2];
        float m33 = this->m[3][3];
        
        float b00 = m00 * m11 - m01 * m10;
        float b01 = m00 * m12 - m02 * m10;
        float b02 = m00 * m13 - m03 * m10;
        float b03 = m01 * m12 - m02 * m11;
        float b04 = m01 * m13 - m03 * m11;
        float b05 = m02 * m13 - m03 * m12;
        float b06 = m20 * m31 - m21 * m30;
        float b07 = m20 * m32 - m22 * m30;
        float b08 = m20 * m33 - m23 * m30;
        float b09 = m21 * m32 - m22 * m31;
        float b10 = m21 * m33 - m23 * m31;
        float b11 = m22 * m33 - m23 * m32;

        float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

        if (det == 0) {
            success = false;
            return result;
        }

        det = 1.0f / det;

        result.m[0][0] = (m11 * b11 - m12 * b10 + m13 * b09) * det;
        result.m[0][1] = (m02 * b10 - m01 * b11 - m03 * b09) * det;
        result.m[0][2] = (m31 * b05 - m32 * b04 + m33 * b03) * det;
        result.m[0][3] = (m22 * b04 - m21 * b05 - m23 * b03) * det;

        result.m[1][0] = (m12 * b08 - m10 * b11 - m13 * b07) * det;
        result.m[1][1] = (m00 * b11 - m02 * b08 + m03 * b07) * det;
        result.m[1][2] = (m32 * b02 - m30 * b05 - m33 * b01) * det;
        result.m[1][3] = (m20 * b05 - m22 * b02 + m23 * b01) * det;

        result.m[2][0] = (m10 * b10 - m11 * b08 + m13 * b06) * det;
        result.m[2][1] = (m01 * b08 - m00 * b10 - m03 * b06) * det;
        result.m[2][2] = (m30 * b04 - m31 * b02 + m33 * b00) * det;
        result.m[2][3] = (m21 * b02 - m20 * b04 - m23 * b00) * det;

        result.m[3][0] = (m11 * b07 - m10 * b09 - m12 * b06) * det;
        result.m[3][1] = (m00 * b09 - m01 * b07 + m02 * b06) * det;
        result.m[3][2] = (m31 * b01 - m30 * b03 - m32 * b00) * det;
        result.m[3][3] = (m20 * b03 - m21 * b01 + m22 * b00) * det;

        success = true;
        return result;
    }

    // Leverages the fact that the inverse of an orthogonal matrix is its transpose.
    // See: https://www.3dgep.com/understanding-the-view-matrix/#look-at-camera
    Matrix4 Matrix4::orthoInverse(bool& success) {
        Matrix4 result{0};

        // The top-left 3x3 submatrix is the transpose of the original.
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                result.m[r][c] = this->m[c][r];

        result.columns.w = {
            -(this->columns.w.dot(this->columns.x)),
            -(this->columns.w.dot(this->columns.y)),
            -(this->columns.w.dot(this->columns.z)),
            1.0f
        };

        return result;
    }

    void Matrix4::print() {
        printf("Matrix4:\n");
        for (int r = 0; r < 4; r++)
            printf("%.4f %.4f %.4f %.4f\n", this->m[0][r], this->m[1][r], this->m[2][r], this->m[3][r]);
    }

    // Static Matrix4 Functions
    Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
        Matrix4 result{0};
        float tanHalfFov = tanf(fov / 2.0f);
        result.m[0][0] = 1.0f / (aspect * tanHalfFov);
        result.m[1][1] = 1.0f / tanHalfFov;
        result.m[2][2] = -(far + near) / (far - near);
        result.m[2][3] = -1.0f;
        result.m[3][2] = -(2.0f * far * near) / (far - near);
        return result;
    }

    Matrix4 Matrix4::camera(Vector4& position, Vector4& forward, Vector4& up, bool leftHanded) {
        Matrix4 result{0};
        if (leftHanded)
            result.columns.x = forward.cross(up);
        else
            result.columns.x = up.cross(forward);
        result.columns.y = up;
        result.columns.z = forward;
        result.columns.w = position;
        result.columns.w.w = 1.0f;
        return result;
    }

}