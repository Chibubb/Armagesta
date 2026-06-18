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

void interactWithWorld(const char BiomeType, Player& player) {
    if (BiomeType == 'F') {
        auto Event = make_unique<Forest>(player);
    }
    if (BiomeType == 'X') {
        cout << "You are at the edge of the map, you may not travel any farther in that direction..." << endl;
    }
}

int main() {



    auto player = make_unique<Player>();

    cout << "Welcome to the Game!" << endl << endl;
    bool gameEnds = false;


     while (gameEnds == false) {
         player->printActions();
         string chosenAction = player->getAction();
         if (chosenAction == "Move") {
             interactWithWorld(player->move(player->getAChosenDirection()), *player);
         }
     }



    return 0;
}

