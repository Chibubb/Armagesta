//
// Created by Taylor Anderson on 6/17/26.
//

#ifndef ARMAGESTA_PLAYERDATABASE_H
#define ARMAGESTA_PLAYERDATABASE_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include <vector>

using namespace std;

class Player {
private:
    int health = 100;
    int maxHealth = 100;
    int mana = 10;
    int maxSoul = 10;
    vector<int> currentPosition = {5, 5};
    vector<vector<string>> actions = {
        {}
    };
    vector<string> actionCategories = {};

public:

    void heal(const int healAmount) {
        health += healAmount;
    }

    void move(string direction) {
        if ()
    }

};

#endif //ARMAGESTA_PLAYERDATABASE_H
