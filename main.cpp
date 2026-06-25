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

void interactWithWorld(const char BiomeType, Player& player, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) {
    if (BiomeType == 'F') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Forest>(player, musicManager);
    } else if (BiomeType == 'D') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Desert>(player, musicManager);
    } else if (BiomeType == 'C') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Caves>(player, musicManager);
    } else if (BiomeType == 'R') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<OldRedwoods>(player, musicManager);
    } else if (BiomeType == 'W') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Swamp>(player, musicManager);
    } else if (BiomeType == 'I') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Citadel>(player, musicManager);
    } else if (BiomeType == 'B') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Beach>(player, musicManager);
    } else if (BiomeType == 'M') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<Mountains>(player, musicManager);
    } else if (BiomeType == 'S') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<SandPit>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'G') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<CrystalGeode>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'O') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<OldestRedwood>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'H') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<WitchsHut>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'T') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<TheThrone>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'Y') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<CoralReef>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'L') {
        musicManager.changeAmbienceForBiome(BiomeType);
        auto Event = make_unique<DragonsLair>(player, musicManager, loreOnlyRevisit);
    } else if (BiomeType == 'X') {
        cout << "You are at the edge of the map, you may not travel any farther in that direction..." << endl;
    }
}


void runWorldEventWithChangeReport(const char biomeType, Player& player, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) {
    const auto beforeEvent = player.makeChangeSnapshot();

    if (biomeType != 'X' && player.isImportantLorePlaceType(biomeType) && !loreOnlyRevisit) {
        musicManager.playCueImmediately(ArmagestaMusic::strangeCue(), 0.75f);
    }

    interactWithWorld(biomeType, player, musicManager, loreOnlyRevisit);
    player.reportChangesSince(beforeEvent);
}

void runNonCombatEventWithChangeReport(Player& player) {
    const auto beforeEvent = player.makeChangeSnapshot();
    ArmagestaNonCombatEvents::maybeTrigger(player);
    player.reportChangesSince(beforeEvent);
}

int main() {

    BackgroundMusicManager musicManager;

    auto player = make_unique<Player>();
    musicManager.changeAmbienceForBiome(player->getCurrentBiomeType());

    cout << endl << player->ansiColor("Welcome to Armagesta!", "1;35") << endl << endl;
    player->chooseDifficultyAtNewGame();
    bool gameEnds = false;


    while (gameEnds == false) {
        player->printActions();
        string chosenAction = player->getAction();

        if (chosenAction == "Move") {
            cout << endl << player->ansiColor("Explored map so far:", "1;35") << endl;
            player->printMap();

            const char previousBiomeType = player->getCurrentBiomeType();
            char biomeType = player->move(player->getAChosenDirection());

            if (biomeType == 'X') {
                runWorldEventWithChangeReport(biomeType, *player, musicManager);
            } else {
                if (player->shouldAnnounceMajorBiomeChange(previousBiomeType, biomeType)) {
                    player->announceMajorBiomeEntrance(biomeType);
                }

                if (player->currentSpaceHasBeenExplored()) {
                    if (player->currentExploredSpaceShouldRunLoreOnlyEvent()) {
                        player->describeLoreOnlyRevisit();
                        runWorldEventWithChangeReport(biomeType, *player, musicManager, true);
                    } else {
                        player->describeAlreadyExploredSpace();
                        runNonCombatEventWithChangeReport(*player);
                    }
                } else {
                    player->markCurrentSpaceExplored();
                    runWorldEventWithChangeReport(biomeType, *player, musicManager);
                    runNonCombatEventWithChangeReport(*player);
                }
            }
        } else if (chosenAction == "Self Assess") {
            player->assess();
        } else if (chosenAction == "Understand Powers") {
            player->understandPowers();
        } else if (chosenAction == "Map") {
            player->printMap();
        } else if (chosenAction == "Quest Log") {
            player->printQuestLog();
        } else if (chosenAction == "Consumed Souls") {
            player->showConsumedMonsterSouls();
        } else if (chosenAction == "Settings Menu") {
            player->settingsMenu();
        } else if (chosenAction == "Save Game") {
            player->saveGame();
        } else if (chosenAction == "Load Game") {
            const auto beforeLoad = player->makeChangeSnapshot();
            if (player->loadGame()) {
                player->reportChangesSince(beforeLoad);
                musicManager.changeAmbienceForBiome(player->getCurrentBiomeType());
            }
        }

        cout << endl;

        if (player->gameWon) {
            musicManager.stopMusicWithFade(0.40f);
            musicManager.playCueImmediately(ArmagestaMusic::completionCue(), 1.20f, false);
            cout << player->ansiColor("You have completed the first version of Armagesta.", "1;35") << endl;
            cout << "The map still " << player->ansiColor("breathes", "1;32") << ", but it no longer does so without a " << player->ansiColor("ruler", "1;35") << "." << endl;
            gameEnds = true;
        }

        if (player->health <= 0) {
            musicManager.stopMusicWithFade(0.40f);
            musicManager.playCueImmediately(ArmagestaMusic::deathCue(), 1.15f, false);
            cout << player->ansiColor("You fall to the floor, breathless. You have died.", "1;31") << endl;
            gameEnds = true;
        }
    }



    return 0;
}

