#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <cmath>

template<typename T>
constexpr T pi() { return 3.1415926535897932384626433832795028841971; }

template<typename T>
constexpr T halfPi() { return pi<T>() / T(2); }

template<typename T>
constexpr T tau() { return pi<T>() * T(2); }

template<typename T>
constexpr T degreesPerRadian() { return T(180) / pi<T>(); }

template<typename T>
constexpr T radiansPerDegree() { return pi<T>() / T(180); }

template<typename T>
class Rotation
{
    T _radians;

    Rotation(T radians) : _radians(radians) {}

public:
    static Rotation fromRadians(T radians)
    {
        radians = std::fmod(radians, tau<T>());
        if (radians < 0.0f) radians += tau<T>();
        return Rotation(radians);
    }

    static Rotation fromDegrees(T degrees)
    {
        return fromRadians(degrees * radiansPerDegree<T>());
    }

    static constexpr const Rotation half()
    {
        return Rotation(pi<T>());
    }

    constexpr Rotation() : _radians(0) {}
    constexpr Rotation(const Rotation&) = default;
    ~Rotation() = default;

    Rotation<T>& operator=(const Rotation<T>&) = default;

    Rotation<T>& operator+=(const Rotation<T>& other)
    {
        _radians += other._radians;
        if (_radians >= tau<T>()) _radians -= tau<T>();
        return *this;
    }

    Rotation<T>& operator-=(const Rotation<T>& other)
    {
        _radians -= other._radians;
        if (_radians < 0.0f) _radians += tau<T>();
        return *this;
    }

    Rotation<T> operator+(const Rotation<T>& other) const
    {
        return Rotation<T>(_radians) += other;
    }

    Rotation<T> operator-(const Rotation<T>& other) const
    {
        return Rotation<T>(_radians) -= other;
    }

    T toRadians() const { return _radians; }
    T toDegrees() const { return _radians * degreesPerRadian<T>(); }
};

typedef Rotation<float> RotationF;
typedef Rotation<double> RotationD;

#endif
