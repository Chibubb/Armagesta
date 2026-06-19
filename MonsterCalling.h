//
// Created by Taylor Anderson on 6/18/26.
//

#ifndef ARMAGESTA_MONSTERCALLING_H
#define ARMAGESTA_MONSTERCALLING_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include <vector>
#include "Encounter.h"
#include "PlayerDatabase.h"

using namespace std;

inline void encounterSlime(Player& playerIP) {
    unique_ptr<Encounter> encounter = make_unique<Slime>(playerIP);
}

#endif //ARMAGESTA_MONSTERCALLING_H
