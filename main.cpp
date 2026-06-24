//
// Created by Taylor Anderson on 6/15/26.
//
#include <iostream>
#include "randomness.h"
#include <memory>
#include "Events.h"
#include <vector>
#include "PlayerDatabase.h"
#include "BackgroundMusicManager.h"

using namespace std;

void interactWithWorld(const char BiomeType, Player& player, BackgroundMusicManager& musicManager) {
    if (BiomeType == 'F') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Forest>(player, musicManager);
    } else if (BiomeType == 'D') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Desert>(player, musicManager);
    } else if (BiomeType == 'C') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Caves>(player, musicManager);
    } else if (BiomeType == 'R') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<OldRedwoods>(player, musicManager);
    } else if (BiomeType == 'W') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Swamp>(player, musicManager);
    } else if (BiomeType == 'I') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Citadel>(player, musicManager);
    } else if (BiomeType == 'B') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Beach>(player, musicManager);
    } else if (BiomeType == 'M') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<Mountains>(player, musicManager);
    } else if (BiomeType == 'S') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<SandPit>(player, musicManager);
    } else if (BiomeType == 'G') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<CrystalGeode>(player, musicManager);
    } else if (BiomeType == 'O') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<OldestRedwood>(player, musicManager);
    } else if (BiomeType == 'H') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<WitchsHut>(player, musicManager);
    } else if (BiomeType == 'T') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<TheThrone>(player, musicManager);
    } else if (BiomeType == 'Y') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<CoralReef>(player, musicManager);
    } else if (BiomeType == 'L') {
        musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");
        auto Event = make_unique<DragonsLair>(player, musicManager);
    } else if (BiomeType == 'X') {
        cout << "You are at the edge of the map, you may not travel any farther in that direction..." << endl;
    }
}

int main() {

    BackgroundMusicManager musicManager;

    musicManager.changeMusicWithFade("assets/music/Light Ambience 1.mp3");

    auto player = make_unique<Player>();

    cout << "Welcome to Armagesta!" << endl << endl;
    bool gameEnds = false;


    while (gameEnds == false) {
        player->printActions();
        string chosenAction = player->getAction();

        if (chosenAction == "Move") {
            char biomeType = player->move(player->getAChosenDirection());

            if (biomeType == 'X') {
                interactWithWorld(biomeType, *player, musicManager);
            } else if (player->currentSpaceHasBeenExplored()) {
                player->describeAlreadyExploredSpace();
                ArmagestaNonCombatEvents::maybeTrigger(*player);
            } else {
                player->markCurrentSpaceExplored();
                interactWithWorld(biomeType, *player, musicManager);
                ArmagestaNonCombatEvents::maybeTrigger(*player);
            }
        } else if (chosenAction == "Self Assess") {
            player->assess();
        } else if (chosenAction == "Understand Powers") {
            player->understandPowers();
        } else if (chosenAction == "Map") {
            player->printMap();
        } else if (chosenAction == "Quest Log") {
            player->printQuestLog();
        } else if (chosenAction == "Save Game") {
            player->saveGame();
        } else if (chosenAction == "Load Game") {
            player->loadGame();
        }

        cout << endl;

        if (player->gameWon) {
            cout << "You have completed the first version of Armagesta." << endl;
            cout << "The map still breathes, but it no longer does so without a ruler." << endl;
            gameEnds = true;
        }

        if (player->health <= 0) {
            cout << "You fall to the floor, breathless. You have died." << endl;
            gameEnds = true;
        }
    }



    return 0;
}

