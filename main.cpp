//
// Created by Taylor Anderson on 6/15/26.
//
#include <iostream>
#include "randomness.h"
#include <memory>

using namespace std;

struct Player {
    int health = 100;
    vector<int> currentPosition = {5, 5};
};

int main() {
    vector<vector<char>> map = {
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {}
    }; // 0 - 10 on both dimensions

    auto playerPointer = make_unique<Player>();


    return 0;
}

