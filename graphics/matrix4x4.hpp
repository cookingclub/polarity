#ifndef POLARITY_GRAPHICS_MATRIX4x4_HPP__
#define POLARITY_GRAPHICS_MATRIX4x4_HPP__

#include <algorithm>
#include <cmath>

namespace Polarity {

template <class Val> class Matrix4x4 {

    enum {
        SIZE = 16
    };
    Val mat[SIZE]; // column major

public:
    Matrix4x4 () {
    }

    Matrix4x4 (std::initializer_list<Val> list) {
        assert(list.size() <= SIZE);
        if (list.size() > SIZE) {
            return;
        }
        std::copy(list.begin(), list.end(), mat);
        std::fill(mat + list.size(), mat + SIZE, 0);
    }

    Val& operator[] (int i) {
        return mat[i];
    }
    Val operator[] (int i) const {
        return mat[i];
    }

    const GLfloat* values() const {
        return &mat[0];
    }

    Matrix4x4 operator* (const Matrix4x4&b) const {
        const Matrix4x4 &a = *this;

        Matrix4x4 c;
        for (int j = 0; j < 16; j += 4) {
            for (int i = 0; i < 4; i++) {
                c[i + j] = a[i] * b[j] + a[i + 4] * b[1 + j] + a[i + 8] * b[2 + j] + a[i + 12] * b[3 + j];
            }
        }
        return c;
    }

    Matrix4x4& operator*= (const Matrix4x4&b) {
        *this = (*this) * b;
        return *this;
    }

    Matrix4x4 operator* (Val scalar) const {
        Matrix4x4 ret;
        for (int i = 0; i < 16; i++) {
            ret[i] = scalar * mat[i];
        }
        return ret;
    }

    Matrix4x4 operator*= (Val scalar) const {
        for (int i = 0; i < 16; i++) {
            mat[i] *= scalar;
        }
        return *this;
    }

    static const Matrix4x4& identity() {
        return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
    }

    static Matrix4x4 rotationZ(Val radians) {
        return {
            std::cosf(radians), -std::sinf(radians), 0, 0,
            std::sinf(radians), std::cosf(radians), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    static Matrix4x4 rotationY(Val radians) {
        return {
            std::cosf(radians), 0, std::sinf(radians), 0,
            0, 1, 0, 0,
            -std::sinf(radians), 0, std::cosf(radians), 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    static Matrix4x4 rotationX(Val radians) {
        return {
            1, 0, 0, 0,
            0, std::cosf(radians), -std::sinf(radians), 0,
            0, std::sinf(radians), std::cosf(radians), 0,
            0, 0, 0, 1
        };
    }

    static Matrix4x4 scalar(Val x, Val y, Val z) {
        return {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1,
        };
    }

    static Matrix4x4 scalar(Val s) {
        return scalar(s, s, s);
    }

    static Matrix4x4 translation(Val x, Val y, Val z) {
        return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        };
    }
};

template <class Val>
std::ostream &operator << (std::ostream &os, const Matrix4x4<Val> &mat) {
    for (int i = 0; i < 16; i += 4) {
        for (int j = 0; j < 4; j++) {
            os << mat[i + j] << ",";
        }
        os << ";";
    }
    return os;
}

typedef Matrix4x4<GLfloat> GLMatrix4x4;

}
#endif
