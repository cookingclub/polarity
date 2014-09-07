#ifndef POLARITY_GRAPHICS_MATRIX4x4_HPP__
#define POLARITY_GRAPHICS_MATRIX4x4_HPP__

#include <algorithm>
#include <cmath>

namespace Polarity {

template <class Val> class Vec4 {
public:
    union {
        Val vec[4];
        struct {
            Val x, y, z, w;
        } coord;
    } u;

    Vec4(Val x, Val y, Val z, Val w) {
        u.coord.x = x;
        u.coord.y = y;
        u.coord.z = z;
        u.coord.w = w;
    }

    Vec4 (std::initializer_list<Val> list) {
        assert(list == 4);
        if (list.size() > 4) {
            return;
        }
        std::copy(list.begin(), list.end(), u.vec);
    }

    Val& operator[] (int i) {
        return u.vec[i];
    }
    Val operator[] (int i) const {
        return u.vec[i];
    }

    const Val* values() const {
        return &u.vec[0];
    }
};

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

    const Val* values() const {
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

    Vec4<Val> operator*(const Vec4<Val> &v) const {
        return Vec4<Val> (
            v[0] * mat[0] + v[1] * mat[4] + v[2] * mat[8] + v[3] * mat[12],
            v[0] * mat[1] + v[1] * mat[5] + v[2] * mat[9] + v[3] * mat[13],
            v[0] * mat[2] + v[1] * mat[6] + v[2] * mat[10] + v[3] * mat[14],
            v[0] * mat[3] + v[1] * mat[7] + v[2] * mat[11] + v[3] * mat[15]);
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
            std::cos(radians), -std::sin(radians), 0, 0,
            std::sin(radians), std::cos(radians), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    static Matrix4x4 rotationY(Val radians) {
        return {
            std::cos(radians), 0, std::sin(radians), 0,
            0, 1, 0, 0,
            -std::sin(radians), 0, std::cos(radians), 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    static Matrix4x4 rotationX(Val radians) {
        return {
            1, 0, 0, 0,
            0, std::cos(radians), -std::sin(radians), 0,
            0, std::sin(radians), std::cos(radians), 0,
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
    os << "Mat4[[";
    for (int i = 0; i < 16; i += 4) {
        i && os << "][";
        for (int j = 0; j < 4; j++) {
            j && os << ",";
            os << mat[i + j];
        }
    }
    os << "]]";
    return os;
}

template <class Val>
std::ostream &operator << (std::ostream &os, const Vec4<Val> &vec) {
    os << "<";
    for (int i = 0; i < 4; i ++) {
        i && os << ",";
        os << vec[i];
    }
    os << ">";
    return os;
}

typedef Vec4<float> GLVec4;
typedef Matrix4x4<float> GLMatrix4x4;

}
#endif
