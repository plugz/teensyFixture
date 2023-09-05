#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <Arduino.h>
#include <chrono>
#include <initializer_list>
#include <array>

//#include <iostream>

#include "FakeFloat.hpp"

struct Utils {
    static void begin();

    // input: [0, 1] instead of [0, 2pi]
    // return: [0, 1] instead of [-1, 1]
    static Float sin1(Float x);
    //static Float cos1(Float x);
    // input: [0, 1]
    // output: [0, 1]
    static Float asin1(Float x);
    //static Float acos1(Float x);
    // input: [0, 2]
    static Float sqrt2(Float x);
};

static auto milliss() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point{})
      .count();
};
static auto micross() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point{})
      .count();
};

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(*array))

using Micros = decltype(micros());
using Millis = decltype(millis());

template<class TTimeType, class TImpl>
class Timer {
public:
    Timer(TTimeType period) {
        _prevTime = TImpl::now();
        _period = period;
    }
    void reset() {
        _prevTime = TImpl::now();
    }
    bool advance() {
        TTimeType now = TImpl::now();
        TTimeType nextTime = _prevTime + _period;
        if (now < _prevTime) {
            // "now" has overflowed
            if (nextTime > _prevTime) {
                // "nextTime" has not overflowed
                _prevTime = nextTime;
                return true;
            }
            if (now >= nextTime) {
                _prevTime = nextTime;
                return true;
            }
            return false;
        }
        if (nextTime < _prevTime) {
            // "nextTime" has overflowed
            return false;
        }
        if (now >= nextTime) {
            _prevTime = nextTime;
            return true;
        }
        return false;
    }

private:
    TTimeType _prevTime;
    TTimeType _period;
};

class TimerMicros : public Timer<Micros, TimerMicros> {
public:
    using Timer<Micros, TimerMicros>::Timer;
    static Micros now() {
        return micros();
    }
};

class TimerMillis : public Timer<Millis, TimerMillis> {
public:
    using Timer<Millis, TimerMillis>::Timer;
    static Millis now() {
        return millis();
    }
};

template <typename T> struct StaticVector {
    StaticVector()
        : data(nullptr)
        , size(0) {
        }

    StaticVector(T* d, size_t s)
        : data(d)
        , size(s) {
        }

    T* data;
    size_t size;
    T& operator[](size_t idx) { return data[idx]; }
    T const& operator[](size_t idx) const { return data[idx]; }

    T* begin() { return data; }
    T* end() { return data + size; }
    T const* begin() const { return data; }
    T const* end() const { return data + size; }

    void clear() {
        data = nullptr;
        size = 0;
    }
};

struct EffectDesc;
struct EffectComboDesc;

//template<typename T> std::ostream& operator <<(std::ostream& s, StaticVector<T> const& v) {
//    return s << (void*)v.data << "." << v.size;
//}

// with memory
template <typename T, size_t TMemorySize> struct StaticVector2 : public StaticVector<T> {

    StaticVector2()
        : StaticVector<T>() {
        }

    StaticVector2(std::initializer_list<T> list) : StaticVector<T>() {

        // static memory
        static std::array<T, TMemorySize> memory{};
        static T* nextMemory = memory.data();

        StaticVector<T>::data = nextMemory;
        StaticVector<T>::size = list.size();

        for (auto const& listElement : list) {
            *nextMemory++ = listElement;
        }
    }
};

#endif
