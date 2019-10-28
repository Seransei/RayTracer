#ifndef IMAGE_SYNTHESIS_VEC3_H
#define IMAGE_SYNTHESIS_VEC3_H

#include <iostream>
#include <cmath>

template<typename T>
struct Vector3 {
    T x, y, z;

    Vector3() {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3(const T &x, const T &y, const T &z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    float dot2() const {
        return this->dot(*this);
    }

    float norm() const {
        return (float)sqrt(this->dot2());
    }

    Vector3<T> normalize() const{
        return *this / this->norm();
    }

    Vector3<T> clamp255(){
        x = (x > 255) ? 255 : (x < 0) ? 0 : x;
        y = (y > 255) ? 255 : (y < 0) ? 0 : y;
        z = (z > 255) ? 255 : (z < 0) ? 0 : z;
        return *this;
    }

    Vector3<T> clamp1(){
        x = (x > 1) ? 1 : (x < 0) ? 0 : x;
        y = (y > 1) ? 1 : (y < 0) ? 0 : y;
        z = (z > 1) ? 1 : (z < 0) ? 0 : z;
        return *this;
    }

    bool operator==(const Vector3 &v) const {
        return x == v.x &&
               y == v.y &&
               z == v.z;
    }

    bool operator!=(const Vector3 &v) const {
        return !(v == *this);
    }

    Vector3<T> operator+(const Vector3 &v) const {
        return Vector3<T>{x + v.x, y + v.y, z + v.z};
    }

    Vector3<T> operator+=(const Vector3 &v) {
        return Vector3<T>{x += v.x, y += v.y, z += v.z};
    }

    Vector3<T> operator-(const Vector3 &v) const {
        return Vector3<T>{x - v.x, y - v.y, z - v.z};
    }

    Vector3<T> operator*(const Vector3 &v) const {
        return Vector3<T>{x * v.x, y * v.y, z * v.z};
    }

    Vector3<T> operator/(const Vector3 &v) const {
        return Vector3<T>{x / v.x, y / v.y, z / v.z};
    }

    Vector3<T> operator/(const float &scalar) const {
        return Vector3<T>{x / scalar, y / scalar, z / scalar};
    }

	friend Vector3<T> operator/(const float &scalar, const Vector3<T> v) {
		return Vector3<T>{scalar / v.x, scalar / v.y, scalar / v.z};
	}

    friend Vector3<T> operator*(const float &scalar, const Vector3<T> &v) {
        return Vector3{scalar * v.x, scalar * v.y, scalar * v.z};
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector3<T> &vector3) {
        return os << "[" << vector3.x << ", " << vector3.y << ", " << vector3.z << "]";
    }
};
#endif
