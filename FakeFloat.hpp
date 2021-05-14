#ifndef __FAKEFLOAT_HPP__
#define __FAKEFLOAT_HPP__

#include <cstdint>
//#include <iostream>

template <typename TType, int TPrecisionBits> struct FakeFloat {
    static constexpr int TPrecisionValue = 1 << TPrecisionBits;
    static constexpr int TPrecisionMask = TPrecisionValue - 1;

    FakeFloat() {}

    explicit FakeFloat(TType v)
        : value(v) {}

    template <typename T> static FakeFloat scaleUp(T v) { return FakeFloat{v * TPrecisionValue}; }

    TType scaleDown() const { return value / TPrecisionValue; }

    FakeFloat operator+(FakeFloat other) const { return FakeFloat{value + other.value}; }
    FakeFloat operator-(FakeFloat other) const { return FakeFloat{value - other.value}; }
    FakeFloat operator*(FakeFloat other) const { return FakeFloat{(value * other.value) / TPrecisionValue}; }
    FakeFloat operator/(FakeFloat other) const { return FakeFloat{(value * TPrecisionValue) / other.value}; }
    FakeFloat operator%(FakeFloat other) const { return FakeFloat{value % other.value}; }
    FakeFloat operator-() const { return FakeFloat{-value}; }

    bool operator<(FakeFloat other) const { return value < other.value; }
    bool operator<=(FakeFloat other) const { return value <= other.value; }
    bool operator>(FakeFloat other) const { return value > other.value; }
    bool operator>=(FakeFloat other) const { return value >= other.value; }
    bool operator==(FakeFloat other) const { return value == other.value; }

    FakeFloat floatPart() const { return FakeFloat{value & TPrecisionMask}; }
    FakeFloat floorPart() const { return FakeFloat{value & ~TPrecisionMask}; }

    FakeFloat operator*(TType other) const { return FakeFloat{value * other}; }
    FakeFloat operator/(TType other) const { return FakeFloat{value / other}; }

    template <typename T> FakeFloat& operator*=(T other) { return *this = *this * other; }
    template <typename T> FakeFloat& operator/=(T other) { return *this = *this / other; }
    template <typename T> FakeFloat& operator%=(T other) { return *this = *this % other; }

    TType value;
};

template <typename T, int TPrec>
FakeFloat<T, TPrec> operator*(int left, FakeFloat<T, TPrec> right) {
    return right * left;
}

using Float = FakeFloat<int32_t, 11>;

#endif
