#ifndef ROTATION_HPP
#define ROTATION_HPP

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
public:
    static constexpr Rotation fromDegrees(T degrees)
    {
        return Rotation(degrees * radiansPerDegree<T>());
    }

    static constexpr Rotation fromRadians(T radians)
    {
        return Rotation(radians);
    }

    static constexpr const Rotation half()
    {
        return Rotation(-pi<T>());
    }

    constexpr Rotation() : _radians(0) {}
    constexpr Rotation(const Rotation&) = default;
    ~Rotation() = default;

    Rotation<T>& operator=(const Rotation<T>& other) = default;

    Rotation<T>& operator+=(const Rotation<T>& other)
    {
        _radians = cycle(_radians + other._radians);
        return *this;
    }

    Rotation<T>& operator-=(const Rotation<T>& other)
    {
        _radians = cycle(_radians - other._radians);
        return *this;
    }

    const Rotation<T> operator+(const Rotation<T>& other) const
    {
        return Rotation<T>(_radians) += other;
    }

    const Rotation<T> operator-(const Rotation<T>& other) const
    {
        return Rotation<T>(_radians) -= other;
    }

    T toRadians() const { return _radians; }
    T toDegrees() const { return _radians * degreesPerRadian<T>(); }

private:
    Rotation(T radians) : _radians(radians) {}

    T _radians;

    static T cycle(T radians)
    {
        if (radians >= pi<T>())
            radians -= tau<T>();
        else if (radians < -pi<T>())
            radians += tau<T>();

        return radians;
    }
};

typedef Rotation<float> RotationF;
typedef Rotation<double> RotationD;

#endif
