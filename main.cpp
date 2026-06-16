//
// Created by Taylor Anderson on 6/15/26.
//
#include <iostream>
#include "randomness.h"
#include <memory>
#include <EventDatabase.h>

using namespace std;

struct Player {
    int health = 100;
    int maxHealth = 100;
    int mana = 10;
    int maxMana = 10;
    vector<int> currentPosition = {5, 5};
    vector<vector<string>> actions = {
        {}
    };
    vector<string> actionCategories = {};
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

    auto playerPointer = make_shared<Player>();


    return 0;
}

