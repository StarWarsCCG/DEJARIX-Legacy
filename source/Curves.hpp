#ifndef CURVES_HPP
#define CURVES_HPP

template<typename T1, typename T2> constexpr T1 Linear(T1 first, T1 last, T2 t)
{
    return (last - first) * t + first;
}

template<typename T1, typename T2> T1 Overshoot(
    T1 first, T1 last, T2 magnitude, T2 t)
{
    auto difference = last - first;

    return
        difference * t + first - sin(t * tau<T2>()) * magnitude * difference;
}

template<typename T1, typename T2> T1 SCurve(T1 first, T1 last, T2 t)
{
    auto difference = last - first;
    auto tt = (T2(1) - cos(t * pi<T2>())) * T2(0.5);
    return difference * tt + first;
}

template<typename T1, typename T2> T1 Boomerang(T1 first, T1 last, T2 t)
{
    auto tt = sin(t * pi<T2>());
    return Linear(first, last, tt);
}

template <typename T, int N> struct ControlPoints
{
    T points[N];
};

template <typename Ta, int N, typename Tb>
Ta Interpolate(ControlPoints<Ta, N> control, Tb t)
{
    for (int i = N; i > 0; --i)
    {
        for (int j = 1; j < i; ++j)
        {
            control.points[j - 1] =
                Linear(control.points[j - 1], control.points[j], t);
        }
    }

    return control.points[0];
}

#endif

