//
// Created by Taylor Anderson on 6/15/26.
//

#ifndef ARMAGESTA_RANDOMNESS_H
#define ARMAGESTA_RANDOMNESS_H
#include <random>

inline int randomNum(const int min, const int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}
#endif //ARMAGESTA_RANDOMNESS_H
