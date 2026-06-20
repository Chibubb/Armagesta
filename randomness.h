//
// Created by Taylor Anderson on 6/15/26.
//

#ifndef ARMAGESTA_RANDOMNESS_H
#define ARMAGESTA_RANDOMNESS_H
#include <vector>
#include <random>

using namespace std;

inline int randomNum(const int min, const int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

inline void makeZeroIfNegative(int& value) {
    if (value < 0) {
        value = 0;
    }
}

struct PAD {
    string type;
    int defenceValue = 0;
};

struct MAD {
    int defenceValue = 0;
};

#endif //ARMAGESTA_RANDOMNESS_H
