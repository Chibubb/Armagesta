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
#include "BackgroundMusicManager.h"

using namespace std;

inline void encounterSlime(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<Slime>(playerIP, musicManager);
    encounter->haveCombat();
}

#endif //ARMAGESTA_MONSTERCALLING_H
