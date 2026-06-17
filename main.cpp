//
// Created by Taylor Anderson on 6/15/26.
//
#include <iostream>
#include "randomness.h"
#include <memory>
#include "Events.h"
#include <vector>
#include "PlayerDatabase.h"

using namespace std;

int main() {
    vector<vector<char>> map = {
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'H', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'},
        {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'}
    }; // 0 - 10 on both dimensions

    auto player = make_unique<Player>();


    return 0;
}

