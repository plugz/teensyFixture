#include "Utils.hpp"

#include <cmath>

//#include <iostream>

#define LOOKUP_SIZE 1024

static Float sin_lookup[LOOKUP_SIZE];
static Float sqrt_lookup[LOOKUP_SIZE];

void Utils::begin() {
    for (int i = 0; i < LOOKUP_SIZE; ++i) {
        double const angle = (double(i) * M_PI * 2.0) / double(LOOKUP_SIZE);
        double const s = std::sin(angle);
        double const s1 = (s + 1.0) * 0.5;
        sin_lookup[i] = Float::scaleUp(s1);
//        std::cout << "sin[" << i << "]=" << sin_lookup[i].value << "   ";
    }

    for (int i = 0; i < LOOKUP_SIZE; ++i) {
        double const val = (double(i) * 2.0) / double(LOOKUP_SIZE);
        sqrt_lookup[i] = Float::scaleUp(std::sqrt(val));
//        std::cout << "sqrt[" << i << "]=" << sqrt_lookup[i].value << "   ";
    }
//    std::cout << "\n";
}

Float Utils::sin1(Float x) {
    int idx = (x * LOOKUP_SIZE).scaleDown();
    idx %= LOOKUP_SIZE;
    return sin_lookup[idx];
}

Float Utils::cos1(Float x) {
    int idx = (x * LOOKUP_SIZE).scaleDown();
    idx %= LOOKUP_SIZE;
    idx = (((LOOKUP_SIZE * 5) / 4) - idx) % LOOKUP_SIZE;
    return sin_lookup[idx];
}

Float Utils::sqrt2(Float x) {
    int idx = (x * LOOKUP_SIZE).scaleDown();
    if (idx < 0)
        idx = 0;
    else if (idx >= LOOKUP_SIZE)
        idx = LOOKUP_SIZE - 1;
    return sqrt_lookup[idx];
}
