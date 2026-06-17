//
// Created by Taylor Anderson on 6/16/26.
//

#ifndef ARMAGESTA_EVENTDATABASE_H
#define ARMAGESTA_EVENTDATABASE_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include <vector>

using namespace std;

struct BiomeEventDatabase {
    virtual ~BiomeEventDatabase() = default;
};

struct Forest : BiomeEventDatabase {
    vector<int> eventChances = {};
    vector<string> eventNames = {};
    vector<string> eventFlavorText = {};

};

#endif //ARMAGESTA_EVENTDATABASE_H
