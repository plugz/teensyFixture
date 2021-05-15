#include "Utils.hpp"

#include <cmath>

//#include <iostream>

#define LOOKUP_SIZE 1024

static Float sin_lookup[LOOKUP_SIZE];
static Float sqrt_lookup[LOOKUP_SIZE];
static Float asin_lookup[LOOKUP_SIZE];

void Utils::begin() {
    for (int i = 0; i < LOOKUP_SIZE; ++i) {
        double const angle = (double(i) * M_PI * 2.0) / double(LOOKUP_SIZE);
        double const s = std::sin(angle);
        double const s1 = (s + 1.0) * 0.5;
        sin_lookup[i] = Float::scaleUp(s1);
//        std::cout << "sin[" << i << "]=" << sin_lookup[i].value << "   ";
    }

    for (int i = 0; i < LOOKUP_SIZE; ++i) {
        double const sinus01 = double(i) / double(LOOKUP_SIZE);
        double const sinus11 = (sinus01 * 2.0) - 1.0;
        double const asinpi = std::asin(sinus11);
        double const asin01 = (asinpi + M_PI * 2) / (M_PI * 2.0);
        asin_lookup[i] = Float::scaleUp(asin01).floatPart();
        //asin_lookup[i] = Float::scaleUp((asinpi / M_PI)).floatPart();
        //std::cout << "asin[" << i << "]=" << asin_lookup[i].value << " s01["<<sinus01<<"] s11["<<sinus11<<"] as["<<asinpi<<"] as01["<<asin01<<"]  |||||| \n";
    }

    for (int i = 0; i < LOOKUP_SIZE; ++i) {
        double const val = (double(i) * 2.0) / double(LOOKUP_SIZE);
        sqrt_lookup[i] = Float::scaleUp(std::sqrt(val));
//        std::cout << "sqrt[" << i << "]=" << double(sqrt_lookup[i].value) / double(Float::TPrecisionValue) << "   ";
    }
    //std::cout << "\n";
}

Float Utils::sin1(Float x) {
    int idx = ((x + Float::scaleUp(2))/*.floatPart()*/ * LOOKUP_SIZE).scaleDown();
    idx %= LOOKUP_SIZE;
    return sin_lookup[idx];
}

Float Utils::asin1(Float x) {
    int idx = (x/*.floatPart()*/ * LOOKUP_SIZE).scaleDown();
    if (idx < 0)
        idx = 0;
    else if (idx >= LOOKUP_SIZE)
        idx = LOOKUP_SIZE - 1;
//    idx %= LOOKUP_SIZE;
    return asin_lookup[idx];
}

//Float Utils::cos1(Float x) {
//    int idx = (x * LOOKUP_SIZE).scaleDown();
//    idx %= LOOKUP_SIZE;
//    idx = (((LOOKUP_SIZE * 5) / 4) - idx) % LOOKUP_SIZE;
//    return sin_lookup[idx];
//}

Float Utils::sqrt2(Float x) {
    int idx = ((x / 2) * LOOKUP_SIZE).scaleDown();
    if (idx < 0)
        idx = 0;
    else if (idx >= LOOKUP_SIZE)
        idx = LOOKUP_SIZE - 1;
    return sqrt_lookup[idx];
}
