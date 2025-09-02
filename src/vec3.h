#pragma once
#include <iostream>
#include <cmath>

class vec3 {
public:
    double x, y, z;

    vec3() {
        x = y = z = 0.0;
    }

    vec3(double a, double b, double c) {
        x = a;
        y = b;
        z = c;
    }

    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    vec3 add(const vec3& v) const {
        return vec3(x + v.x, y + v.y, z + v.z);
    }

    vec3 subtract(const vec3& v) const {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    double dot(const vec3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    vec3 operator+(const vec3& v) const {
        return add(v);
    }

    vec3 operator-(const vec3& v) const {
        return subtract(v);
    }

    vec3 operator*(double scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    vec3 operator/(double scalar) const {
        return vec3(x / scalar, y / scalar, z / scalar);
    }

    vec3 cross(const vec3& v) const {
        return vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    void print() const {
        std::cout << x << " " << y << " " << z << "\n";
    }
};