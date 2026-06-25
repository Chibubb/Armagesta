//
// Created by Taylor Anderson on 6/17/26.
//

#ifndef ARMAGESTA_PLAYERDATABASE_H
#define ARMAGESTA_PLAYERDATABASE_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include "Events.h"
#include <memory>
#include <vector>
#include "BackgroundMusicManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

class Player {
private:
public:

    bool currentSpaceHasBeenExplored() const {
        return exploredMap[currentPosition[0]][currentPosition[1]];
    }

    void markCurrentSpaceExplored() {
        exploredMap[currentPosition[0]][currentPosition[1]] = true;
    }

    char getCurrentBiomeType() const {
        return map[currentPosition[0]][currentPosition[1]];
    }

    bool isMajorBiomeType(const char biomeType) const {
        return biomeType == 'F' || biomeType == 'D' || biomeType == 'C' || biomeType == 'R' ||
               biomeType == 'W' || biomeType == 'I' || biomeType == 'B' || biomeType == 'M';
    }

    char getMajorBiomeType(const char biomeType) const {
        // Landmark biomes inherit the region they are hidden inside. This prevents the big
        // biome entrance banner from firing when stepping onto or off of one-space places.
        if (biomeType == 'S') return 'D'; // Sand Pit lives in the Desert.
        if (biomeType == 'G') return 'C'; // Crystal Geode lives in the Caves.
        if (biomeType == 'O') return 'R'; // Oldest Redwood lives in the Old Redwoods.
        if (biomeType == 'H') return 'W'; // Witch's Hut lives in the Swamp.
        if (biomeType == 'T') return 'I'; // The Throne lives in the Citadel.
        if (biomeType == 'Y') return 'B'; // Coral Reef lives in the Beach region.
        if (biomeType == 'L') return 'M'; // Dragon's Lair lives in the Mountains.
        return biomeType;
    }

    string getBiomeNameFromType(const char biomeType) const {
        if (biomeType == 'F') return "Forest";
        if (biomeType == 'D') return "Desert";
        if (biomeType == 'C') return "Caves";
        if (biomeType == 'R') return "Old Redwoods";
        if (biomeType == 'W') return "Swamp";
        if (biomeType == 'I') return "Citadel";
        if (biomeType == 'B') return "Beach";
        if (biomeType == 'M') return "Mountains";

        if (biomeType == 'S') return "Sand Pit";
        if (biomeType == 'G') return "Crystal Geode";
        if (biomeType == 'O') return "Oldest Redwood";
        if (biomeType == 'H') return "Witch's Hut";
        if (biomeType == 'T') return "The Throne";
        if (biomeType == 'Y') return "Coral Reef";
        if (biomeType == 'L') return "Dragon's Lair";

        return "Unknown Place";
    }

    string getCurrentBiomeName() const {
        return getBiomeNameFromType(getCurrentBiomeType());
    }

    string getMajorBiomeNameFromType(const char biomeType) const {
        return getBiomeNameFromType(getMajorBiomeType(biomeType));
    }

    bool shouldAnnounceMajorBiomeChange(const char previousBiomeType, const char newBiomeType) const {
        if (newBiomeType == 'X') {
            return false;
        }

        const char previousMajorBiome = getMajorBiomeType(previousBiomeType);
        const char newMajorBiome = getMajorBiomeType(newBiomeType);

        return isMajorBiomeType(newMajorBiome) && previousMajorBiome != newMajorBiome;
    }

    void announceMajorBiomeEntrance(const char biomeType) const {
        const char majorBiomeType = getMajorBiomeType(biomeType);
        const string majorBiomeName = getBiomeNameFromType(majorBiomeType);

        cout << endl;
        cout << "============================================================" << endl;
        cout << "YOU HAVE ENTERED THE " << majorBiomeName << endl;
        cout << "============================================================" << endl;

        if (majorBiomeType == 'F') {
            cout << "The air turns green and watchful. Branches bend like witnesses as the Forest accepts your footsteps." << endl;
        } else if (majorBiomeType == 'D') {
            cout << "The world opens into gold heat and bone-white distance. The Desert measures you by what you can survive without." << endl;
        } else if (majorBiomeType == 'C') {
            cout << "Stone closes overhead. The Caves breathe mineral darkness, and every echo sounds older than your name." << endl;
        } else if (majorBiomeType == 'R') {
            cout << "The trees become impossibly ancient. The Old Redwoods do not welcome you; they remember you in advance." << endl;
        } else if (majorBiomeType == 'W') {
            cout << "Mud, fog, and black water swallow the horizon. The Swamp listens with a thousand hidden throats." << endl;
        } else if (majorBiomeType == 'I') {
            cout << "Broken towers rise like iron teeth. The Citadel still believes in law, even after forgetting mercy." << endl;
        } else if (majorBiomeType == 'B') {
            cout << "Salt wind cuts through the silence. The Beach stretches before you, where every tide returns with evidence." << endl;
        } else if (majorBiomeType == 'M') {
            cout << "The ground climbs toward storm and old fire. The Mountains dare you to become heavy enough to matter." << endl;
        }

        cout << "============================================================" << endl << endl;
    }

    void describeAlreadyExploredSpace() const {
        cout << "You return to the " << getCurrentBiomeName() << "." << endl;
        cout << "The land here has already given up its secret. You may pass through, "
             << "but there is nothing new to uncover at this coordinate." << endl;
    }

    vector<vector<char>> map = {
        {'M', 'M', 'M', 'M', 'L', 'M', 'M', 'R', 'R', 'O', 'R'},
        {'M', 'M', 'M', 'C', 'C', 'G', 'C', 'R', 'R', 'R', 'R'},
        {'M', 'M', 'C', 'C', 'C', 'C', 'C', 'R', 'R', 'R', 'R'},
        {'D', 'D', 'D', 'C', 'F', 'F', 'F', 'W', 'W', 'W', 'W'},
        {'D', 'D', 'S', 'D', 'F', 'F', 'F', 'W', 'W', 'H', 'W'},
        {'D', 'D', 'D', 'F', 'F', 'F', 'F', 'F', 'W', 'W', 'W'},
        {'B', 'B', 'D', 'F', 'F', 'F', 'F', 'I', 'I', 'I', 'I'},
        {'B', 'B', 'B', 'B', 'F', 'F', 'I', 'I', 'I', 'T', 'I'},
        {'Y', 'B', 'B', 'B', 'B', 'I', 'I', 'I', 'I', 'I', 'I'},
        {'Y', 'Y', 'B', 'B', 'B', 'B', 'I', 'I', 'I', 'I', 'I'},
        {'Y', 'Y', 'Y', 'B', 'B', 'B', 'B', 'I', 'I', 'I', 'I'}
    }; // 0 - 10 on both dimensions
    vector<vector<bool>> exploredMap;

    vector<string> activeQuests;
    vector<string> completedQuests;
    vector<string> discoveredLandmarks;
    vector<string> storyFlags;
    vector<string> consumedMonsterSouls;

    bool mainQuestStarted = false;
    bool dragonDefeated = false;
    bool throneClaimed = false;
    bool gameWon = false;

    int health = 100;
    int critChance = 5;
    int accuracy = 80;
    int hardiness = 0;
    int permanentDamageModifier = 0;
    int permanentMomentum = 2;
    int maxHealth = 100;
    int soul = 10;
    int maxSoul = 10;
    int currentScrapMetal = 0;
    vector<int> currentPosition = {5, 5};

    vector<string> actions = {"Move", "Self Assess", "Understand Powers", "Map"};
    vector<string> combatActions = {"Slash", "Eviscerate", "Roll", "Brace", "Think", "Riposte", "Soul Burst"};
    vector<string> combatActionsDescriptions = {"Use your sword on the enemy, like you have done so many times before"
        , "Make the enemy's innards outards, requires a large amount of Momentum"
        , "Try to dodge your enemy's attack by rolling, requires miniscule Momentum"
        , "Block incoming damage using known battle stances"
        , "Take a moment of respite, and assess the situation around you, gains you Momentum"
        , "Parry an enemy who is choosing to attack you, dealing damage, acquiring defense, and gaining Momentum"
        , "Unleash the power of stolen souls on the currently living enemy in front of you, dealing a large amount of damage, requires a decent amount of souls"};

    Player() {
        if (exploredMap.empty()) {
            exploredMap = vector<vector<bool>>(map.size(), vector<bool>(map[0].size(), false));
        }

        markCurrentSpaceExplored();

        unlockWorldAction("Map");
        unlockWorldAction("Quest Log");
        unlockWorldAction("Consumed Souls");
        unlockWorldAction("Save Game");
        unlockWorldAction("Load Game");

        initializeMainQuest();
    }

    void heal(const int healAmount) {
        health += healAmount;
        if (health > maxHealth) {
            health = maxHealth;
        }
    }

    void gainSoul(const int soulAmount) {
        soul += soulAmount;
        if (soul > maxSoul) {
            soul = maxSoul;
        }
    }

    char move(const string& direction) {

        if (direction == "west" || direction == "West") {
            if (currentPosition[1] != 0) {
                currentPosition[1] -= 1;
                return map[currentPosition[0]][currentPosition[1]];
            }else {
                return 'X';
            }
        }else if (direction == "east" || direction == "East") {
            if (currentPosition[1] != 10) {
                currentPosition[1] += 1;
                return map[currentPosition[0]][currentPosition[1]];
            }else {
                return 'X';
            }
        }else if (direction == "north" || direction == "North") {
            if (currentPosition[0] != 0) {
                currentPosition[0] -= 1;
                return map[currentPosition[0]][currentPosition[1]];
            }else {
                return 'X';
            }
        }else if (direction == "south" || direction == "South") {
            if (currentPosition[0] != 10) {
                currentPosition[0] += 1;
                return map[currentPosition[0]][currentPosition[1]];
            }else {
                return 'X';
            }
        }
        return 'X';
    }

    void assess() const {
        cout << "Health Remaining: " << health << " / " << maxHealth << endl;
        cout << "Luck: " << critChance << endl;
        cout << "Accuracy: " << accuracy << endl;
        cout << "Souls: " << soul << " / " << maxSoul << endl;
        cout << "Hardiness: " << hardiness << endl;
        cout << "Strength: " << permanentDamageModifier << endl;
        cout << "Consumed Monster Souls: " << consumedMonsterSouls.size() << endl;
    }

    void understandPowers() {
        for (int i = 0; i < combatActions.size(); i++) {
            cout << i + 1 << ": " << combatActions[i] << endl;
            cout << combatActionsDescriptions[i] << endl << endl;
        }
    }

    string getAChosenDirection() {
        string chosenDirection;
        bool directionChosenCorrectly = false;
        while (directionChosenCorrectly == false) {
            cout << "Which Direction?" << endl;
            getline(cin, chosenDirection);
            if (chosenDirection == "North" || chosenDirection == "north"
                || chosenDirection == "South" || chosenDirection == "south"
                || chosenDirection == "West" || chosenDirection == "west"
                || chosenDirection == "East" || chosenDirection == "east") {
                directionChosenCorrectly = true;
                } else {
                    cout << "Not A Possible Direction..." << endl << endl;
                }
        }
        return chosenDirection;
    }

    void printActions() const {
        cout << "Possible Actions:" << endl;
        for (int i = 0; i < actions.size(); i++) {
            cout << i + 1 << ": " << actions[i] << "  ";
        }
        cout << endl << endl;
    }

    //Map Action NOTE
    string getAction() const {
        bool x = true;
        string chosenAction;
        while (x == true) {
            getline(cin, chosenAction);
            for (const auto & action : actions) {
                if (chosenAction == action) {
                    x = false;
                }
            }
            if (x == true) {
                cout << "Not A Possible Action..." << endl;
            }
        }

        return chosenAction;

    }

    string getCombatAction() const {
        bool x = true;
        string chosenAction;
        while (x == true) {
            cout << "Possible Combat Actions:" << endl;
            for (int i = 0; i < combatActions.size(); i++) {
                cout << i + 1 << ": " << combatActions[i] << "  ";
            }
            cout << endl << endl;
            getline(cin, chosenAction);
            for (const auto & action : combatActions) {
                if (chosenAction == action) {
                    x = false;
                }
            }
            if (x == true) {
                cout << "Not A Possible Action..." << endl;
            }
        }

        return chosenAction;
    }

    void printMap() const {
        cout << "Map:" << endl;
        cout << "@ = You, ? = Unexplored" << endl << endl;

        for (int i = 0; i < map.size(); i++) {
            for (int j = 0; j < map[i].size(); j++) {
                if (currentPosition[0] == i && currentPosition[1] == j) {
                    cout << "@ ";
                } else if (exploredMap[i][j]) {
                    cout << map[i][j] << " ";
                } else {
                    cout << "? ";
                }
            }
            cout << endl;
        }

        cout << endl;
    }

    bool vectorContains(const vector<string>& values, const string& target) const {
    return find(values.begin(), values.end(), target) != values.end();
}

void eraseFromVector(vector<string>& values, const string& target) {
    values.erase(remove(values.begin(), values.end(), target), values.end());
}


struct PlayerChangeSnapshot {
    int health = 0;
    int maxHealth = 0;
    int soul = 0;
    int maxSoul = 0;
    int currentScrapMetal = 0;
    int critChance = 0;
    int accuracy = 0;
    int hardiness = 0;
    int permanentDamageModifier = 0;
    int permanentMomentum = 0;
    bool dragonDefeated = false;
    bool throneClaimed = false;
    bool gameWon = false;
    vector<string> actions;
    vector<string> combatActions;
    vector<string> activeQuests;
    vector<string> completedQuests;
    vector<string> discoveredLandmarks;
    vector<string> storyFlags;
    vector<string> consumedMonsterSouls;
};

PlayerChangeSnapshot makeChangeSnapshot() const {
    PlayerChangeSnapshot snapshot;
    snapshot.health = health;
    snapshot.maxHealth = maxHealth;
    snapshot.soul = soul;
    snapshot.maxSoul = maxSoul;
    snapshot.currentScrapMetal = currentScrapMetal;
    snapshot.critChance = critChance;
    snapshot.accuracy = accuracy;
    snapshot.hardiness = hardiness;
    snapshot.permanentDamageModifier = permanentDamageModifier;
    snapshot.permanentMomentum = permanentMomentum;
    snapshot.dragonDefeated = dragonDefeated;
    snapshot.throneClaimed = throneClaimed;
    snapshot.gameWon = gameWon;
    snapshot.actions = actions;
    snapshot.combatActions = combatActions;
    snapshot.activeQuests = activeQuests;
    snapshot.completedQuests = completedQuests;
    snapshot.discoveredLandmarks = discoveredLandmarks;
    snapshot.storyFlags = storyFlags;
    snapshot.consumedMonsterSouls = consumedMonsterSouls;
    return snapshot;
}

string playerChangeColor() const {
    // Grayish blue ANSI true-color. CLion's Run console supports ANSI coloring.
    return "\033[38;2;135;165;190m";
}

string playerChangeResetColor() const {
    return "\033[0m";
}

void printPlayerChangeLine(const string& message) const {
    cout << playerChangeColor() << message << playerChangeResetColor() << endl;
}

string signedDelta(const int before, const int after) const {
    const int delta = after - before;
    if (delta > 0) {
        return "+" + to_string(delta);
    }
    return to_string(delta);
}

bool containsValue(const vector<string>& values, const string& target) const {
    return find(values.begin(), values.end(), target) != values.end();
}

void reportNumericChange(const string& statName, const int before, const int after, bool& anyChange) const {
    if (before == after) {
        return;
    }

    if (!anyChange) {
        cout << endl;
        printPlayerChangeLine("===== PLAYER CHANGES =====");
        anyChange = true;
    }

    printPlayerChangeLine("- " + statName + ": " + to_string(before) + " -> " + to_string(after) + " (" + signedDelta(before, after) + ")");
}

void reportAddedValues(const string& label, const vector<string>& before, const vector<string>& after, bool& anyChange) const {
    for (const auto& value : after) {
        if (!containsValue(before, value)) {
            if (!anyChange) {
                cout << endl;
                printPlayerChangeLine("===== PLAYER CHANGES =====");
                anyChange = true;
            }
            printPlayerChangeLine("- " + label + ": " + value);
        }
    }
}

void reportRemovedValues(const string& label, const vector<string>& before, const vector<string>& after, bool& anyChange) const {
    for (const auto& value : before) {
        if (!containsValue(after, value)) {
            if (!anyChange) {
                cout << endl;
                printPlayerChangeLine("===== PLAYER CHANGES =====");
                anyChange = true;
            }
            printPlayerChangeLine("- " + label + ": " + value);
        }
    }
}

void reportFlagChange(const string& label, const bool before, const bool after, bool& anyChange) const {
    if (before == after) {
        return;
    }

    if (!anyChange) {
        cout << endl;
        printPlayerChangeLine("===== PLAYER CHANGES =====");
        anyChange = true;
    }

    printPlayerChangeLine("- " + label + ": " + string(after ? "Yes" : "No"));
}

void reportChangesSince(const PlayerChangeSnapshot& before) const {
    bool anyChange = false;

    reportNumericChange("Health", before.health, health, anyChange);
    reportNumericChange("Max Health", before.maxHealth, maxHealth, anyChange);
    reportNumericChange("Souls", before.soul, soul, anyChange);
    reportNumericChange("Max Souls", before.maxSoul, maxSoul, anyChange);
    reportNumericChange("Scrap Metal", before.currentScrapMetal, currentScrapMetal, anyChange);
    reportNumericChange("Luck", before.critChance, critChance, anyChange);
    reportNumericChange("Accuracy", before.accuracy, accuracy, anyChange);
    reportNumericChange("Hardiness", before.hardiness, hardiness, anyChange);
    reportNumericChange("Strength", before.permanentDamageModifier, permanentDamageModifier, anyChange);
    reportNumericChange("Base Momentum", before.permanentMomentum, permanentMomentum, anyChange);

    reportAddedValues("New world action", before.actions, actions, anyChange);
    reportAddedValues("New combat action", before.combatActions, combatActions, anyChange);
    reportAddedValues("Consumed soul", before.consumedMonsterSouls, consumedMonsterSouls, anyChange);
    reportAddedValues("Story milestone", before.storyFlags, storyFlags, anyChange);

    reportFlagChange("Cinder Dragon defeated", before.dragonDefeated, dragonDefeated, anyChange);
    reportFlagChange("Throne claimed", before.throneClaimed, throneClaimed, anyChange);
    reportFlagChange("Game won", before.gameWon, gameWon, anyChange);

    if (anyChange) {
        cout << endl;
    }
}

bool hasWorldAction(const string& actionName) const {
    return vectorContains(actions, actionName);
}

void unlockWorldAction(const string& actionName) {
    if (!hasWorldAction(actionName)) {
        actions.emplace_back(actionName);
    }
}

bool hasCombatAction(const string& actionName) const {
    return vectorContains(combatActions, actionName);
}

void unlockCombatAction(const string& actionName, const string& description) {
    if (hasCombatAction(actionName)) {
        printPlayerChangeLine("You already know " + actionName + ".");
        return;
    }

    combatActions.emplace_back(actionName);
    combatActionsDescriptions.emplace_back(description);
    printPlayerChangeLine("NEW COMBAT ACTION UNLOCKED: " + actionName);
    printPlayerChangeLine(description);
}

int getCombatActionIndex(const string& actionName) const {
    for (int i = 0; i < combatActions.size(); i++) {
        if (combatActions[i] == actionName) {
            return i;
        }
    }
    return -1;
}

void removeCombatAction(const string& actionName) {
    const int index = getCombatActionIndex(actionName);
    if (index < 0) {
        return;
    }

    combatActions.erase(combatActions.begin() + index);
    if (index < combatActionsDescriptions.size()) {
        combatActionsDescriptions.erase(combatActionsDescriptions.begin() + index);
    }
}

void updateCombatActionDescription(const string& actionName, const string& newDescription) {
    const int index = getCombatActionIndex(actionName);
    if (index >= 0 && index < combatActionsDescriptions.size()) {
        combatActionsDescriptions[index] = newDescription;
    }
}

bool hasConsumedMonsterSoul(const string& monsterName) const {
    return vectorContains(consumedMonsterSouls, monsterName);
}

void printSoulRewardHeader(const string& monsterName) const {
    cout << endl;
    printPlayerChangeLine("===== SOUL CONSUMED: " + monsterName + " =====");
}

void showConsumedMonsterSouls() const {
    cout << endl;
    cout << "===== CONSUMED MONSTER SOULS =====" << endl;
    if (consumedMonsterSouls.empty()) {
        cout << "You have not consumed the soul of any monster yet." << endl;
        cout << "The first time you defeat each kind of enemy, its soul will leave a permanent mark on you." << endl;
        cout << endl;
        return;
    }

    for (int i = 0; i < consumedMonsterSouls.size(); i++) {
        cout << i + 1 << ": " << consumedMonsterSouls[i] << endl;
    }
    cout << endl;
    cout << "Each listed soul has already given its first-time reward." << endl;
    cout << "Defeating that same kind of enemy again still grants ordinary Souls, but not another permanent soul-consumption reward." << endl;
    cout << endl;
}

void normalizePlayerStateAfterLoad() {
    removeCombatAction("Weaken");
    unlockWorldAction("Consumed Souls");

    while (combatActionsDescriptions.size() < combatActions.size()) {
        combatActionsDescriptions.emplace_back("No description has been recorded for this power yet.");
    }
    while (combatActionsDescriptions.size() > combatActions.size()) {
        combatActionsDescriptions.pop_back();
    }
}

void consumeMonsterSoul(const string& monsterName) {
    if (hasConsumedMonsterSoul(monsterName)) {
        return;
    }

    consumedMonsterSouls.emplace_back(monsterName);
    printSoulRewardHeader(monsterName);

    if (monsterName == "Slime") {
        maxHealth += 3;
        heal(3);
        printPlayerChangeLine("The Slime's soft body-memory teaches your wounds to close with less argument. Max Health +3 and you heal 3 Health.");
    } else if (monsterName == "Skeleton") {
        accuracy += 1;
        maxSoul += 1;
        printPlayerChangeLine("The Skeleton leaves behind the patience of old bones and empty eye sockets. Accuracy +1 and Max Souls +1.");
    } else if (monsterName == "Thorn Imp") {
        hardiness += 1;
        critChance += 1;
        printPlayerChangeLine("The Thorn Imp's spite roots under your skin. Hardiness +1 and Luck +1.");
    } else if (monsterName == "Mire Leech") {
        maxHealth += 2;
        gainSoul(1);
        printPlayerChangeLine("The Mire Leech's hunger teaches your body how to take back what was stolen. Max Health +2 and you regain 1 Soul.");
    } else if (monsterName == "Glass Spider") {
        accuracy += 2;
        printPlayerChangeLine("The Glass Spider's many reflected eyes settle behind your own. Accuracy +2.");
    } else if (monsterName == "Iron Scarab") {
        hardiness += 1;
        maxHealth += 4;
        printPlayerChangeLine("The Iron Scarab curls into the shape of armor inside your chest. Hardiness +1 and Max Health +4.");
    } else if (monsterName == "Echo Bat") {
        accuracy += 1;
        critChance += 1;
        printPlayerChangeLine("The Echo Bat's last shriek becomes a map of openings. Accuracy +1 and Luck +1.");
    } else if (monsterName == "Ashen Hound") {
        maxSoul += 1;
        permanentMomentum += 1;
        printPlayerChangeLine("The Ashen Hound leaves a coal-red hunting instinct in your legs. Max Souls +1 and Base Momentum +1.");
        unlockCombatAction("Ashen Pursuit", "Spend 2 Souls to chase through an incoming attack, gaining defense and burning the enemy. Stronger when the enemy is attacking.");
    } else if (monsterName == "Grave Moss") {
        hardiness += 1;
        maxHealth += 5;
        printPlayerChangeLine("The Grave Moss settles over your scars like patient soil. Hardiness +1 and Max Health +5.");
        unlockCombatAction("Grave Bloom", "Spend 3 Souls to heal and grow defensive moss. If the enemy is not attacking, you also gain Momentum.");
    } else if (monsterName == "Hollow Squire") {
        hardiness += 2;
        printPlayerChangeLine("The Hollow Squire's drilled reflexes correct your stance. Hardiness +2.");
        printPlayerChangeLine("BRACE UPGRADED: Squire's Brace. Brace now gains extra defense and can create Momentum when used against attacks.");
        updateCombatActionDescription("Brace", "Block incoming damage using disciplined battle stances. The Hollow Squire soul gives this extra defense and Momentum against attacks.");
    } else if (monsterName == "Lantern Bearer") {
        maxSoul += 2;
        printPlayerChangeLine("The Lantern Bearer's little light refuses to go out inside you. Max Souls +2.");
        unlockCombatAction("Lantern Mark", "Spend 2 Souls to mark the enemy with corpse-light, making your next attacks hit harder and lowering its Danger.");
    } else if (monsterName == "Reforged Knight") {
        permanentDamageModifier += 1;
        maxHealth += 5;
        printPlayerChangeLine("The Reforged Knight's furnace-tempered will hammers itself into your sword arm. Strength +1 and Max Health +5.");
        unlockCombatAction("Gravebreaker", "Spend 2 Souls and 4 Momentum on a heavy anti-armor strike that ignores much of the enemy's defense.");
    } else if (monsterName == "Candle Wraith") {
        critChance += 2;
        maxSoul += 1;
        printPlayerChangeLine("The Candle Wraith gutters out, but its last flame learns your name. Luck +2 and Max Souls +1.");
        unlockCombatAction("Wick Drain", "Spend 3 Souls to burn the enemy with ghost-flame, lower its Danger, and draw back a little Soul when the flame bites deep.");
    } else if (monsterName == "Cathedral Gargoyle") {
        hardiness += 2;
        permanentMomentum += 1;
        printPlayerChangeLine("The Cathedral Gargoyle gives you the patience of stone and the violence of falling towers. Hardiness +2 and Base Momentum +1.");
        unlockCombatAction("Stonebound Stance", "Spend 2 Souls to become nearly immovable for one turn, gaining strong defense and punishing melee attackers.");
    } else if (monsterName == "Silt Hydra") {
        maxHealth += 8;
        printPlayerChangeLine("The Silt Hydra's many-throated survival coils into your spine. Max Health +8.");
        unlockCombatAction("Severing Rhythm", "Spend 3 Souls and 3 Momentum to strike in a repeating pattern, dealing several smaller hits that punish wounded enemies.");
    } else if (monsterName == "Starless Oracle") {
        accuracy += 2;
        maxSoul += 2;
        printPlayerChangeLine("The Starless Oracle leaves behind the cold certainty of things that almost happened. Accuracy +2 and Max Souls +2.");
        unlockCombatAction("Foresight", "Spend 3 Souls to read the next instant. If the enemy attacks, the attack fails; otherwise you gain Momentum and sharpen your next strike.");
    } else if (monsterName == "Dune Maw") {
        maxSoul += 3;
        permanentMomentum += 1;
        printPlayerChangeLine("The Dune Maw's endless appetite hollows a useful chamber in your soul. Max Souls +3 and Base Momentum +1.");
        printPlayerChangeLine("BOSS SOUL SHIFT: Sand-hunger. You now begin combat with extra Momentum, and Sandstep costs 1 less Soul.");
    } else if (monsterName == "Crystal Matriarch") {
        maxSoul += 4;
        hardiness += 2;
        printPlayerChangeLine("The Crystal Matriarch fractures into a crown of clear instincts. Max Souls +4 and Hardiness +2.");
        printPlayerChangeLine("BOSS SOUL SHIFT: Prismatic Aegis. Once per combat, a killing blow can leave you barely alive instead.");
    } else if (monsterName == "Ancient Ent") {
        maxHealth += 20;
        hardiness += 2;
        printPlayerChangeLine("The Ancient Ent does not die quickly; it becomes a ring in you. Max Health +20 and Hardiness +2.");
        printPlayerChangeLine("BOSS SOUL SHIFT: Rooted Thought. Think now restores a little Health and grants more Momentum.");
    } else if (monsterName == "Marionette Coven") {
        critChance += 3;
        accuracy += 2;
        printPlayerChangeLine("The Marionette Coven's cut strings wrap around your fingers instead of your throat. Luck +3 and Accuracy +2.");
        printPlayerChangeLine("BOSS SOUL SHIFT: String-Sight. Riposte can now answer quick and ranged attacks, though ranged counters are weaker.");
    } else if (monsterName == "Tide Leviathan") {
        maxSoul += 5;
        heal(10);
        printPlayerChangeLine("The Tide Leviathan leaves an ocean where your limit used to be. Max Souls +5 and you heal 10 Health.");
        printPlayerChangeLine("BOSS SOUL SHIFT: Undertow. Tidecall becomes cheaper and heals you when it crashes through an enemy.");
    } else if (monsterName == "Cinder Dragon") {
        maxSoul += 6;
        permanentDamageModifier += 2;
        maxHealth += 10;
        printPlayerChangeLine("The Cinder Dragon's heart does not fit inside you cleanly; it burns new room. Max Souls +6, Strength +2, and Max Health +10.");
        printPlayerChangeLine("BOSS SOUL SHIFT: Dragonheart. Soul Burst becomes cheaper and far more destructive.");
    } else {
        maxSoul += 1;
        printPlayerChangeLine("This unfamiliar soul leaves a small, nameless widening inside you. Max Souls +1.");
    }

    if (soul > maxSoul) {
        soul = maxSoul;
    }
    if (health > maxHealth) {
        health = maxHealth;
    }

    cout << endl;
}

bool hasStoryFlag(const string& flagName) const {
    return vectorContains(storyFlags, flagName);
}

void addStoryFlag(const string& flagName) {
    if (!hasStoryFlag(flagName)) {
        storyFlags.emplace_back(flagName);
    }
}

bool hasActiveQuest(const string& questName) const {
    return vectorContains(activeQuests, questName);
}

bool hasCompletedQuest(const string& questName) const {
    return vectorContains(completedQuests, questName);
}

void addQuest(const string& questName) {
    if (hasActiveQuest(questName) || hasCompletedQuest(questName)) {
        return;
    }

    activeQuests.emplace_back(questName);
    cout << endl;
    printPlayerChangeLine("NEW OBJECTIVE: " + questName);
}

void completeQuest(const string& questName) {
    if (hasCompletedQuest(questName)) {
        return;
    }

    eraseFromVector(activeQuests, questName);
    completedQuests.emplace_back(questName);
    cout << endl;
    printPlayerChangeLine("OBJECTIVE COMPLETE: " + questName);
}

void initializeMainQuest() {
    if (mainQuestStarted) {
        return;
    }

    mainQuestStarted = true;
    addQuest("Find the seven named places of Armagesta");
    addQuest("Find The Throne");
    addQuest("Find Dragon's Lair");
}

bool hasDiscoveredLandmark(const string& landmarkName) const {
    return vectorContains(discoveredLandmarks, landmarkName);
}

void discoverLandmark(const string& landmarkName) {
    if (hasDiscoveredLandmark(landmarkName)) {
        return;
    }

    discoveredLandmarks.emplace_back(landmarkName);
    cout << endl;
    printPlayerChangeLine("LANDMARK DISCOVERED: " + landmarkName);

    if (landmarkName == "The Throne") {
        completeQuest("Find The Throne");
    }

    if (landmarkName == "Dragon's Lair") {
        completeQuest("Find Dragon's Lair");
        addQuest("Defeat the Cinder Dragon");
    }

    if (discoveredLandmarks.size() >= 7) {
        completeQuest("Find the seven named places of Armagesta");
    }
}

void markDragonDefeated() {
    if (dragonDefeated) {
        return;
    }

    dragonDefeated = true;
    addStoryFlag("Cinder Dragon Defeated");
    completeQuest("Defeat the Cinder Dragon");
    addQuest("Return to The Throne and claim Armagesta");
}

void claimThrone() {
    throneClaimed = true;
    addStoryFlag("Throne Claimed");

    if (dragonDefeated) {
        completeQuest("Return to The Throne and claim Armagesta");
        gameWon = true;
        printPlayerChangeLine("THE THRONE HAS BEEN CLAIMED.");
        cout << endl;
        cout << "The Throne accepts you." << endl;
        cout << "The Dragon is dead, the seven places have spoken, and Armagesta finally has a will again." << endl;
    } else {
        cout << endl;
        cout << "The Throne burns cold beneath you. It will not fully accept a ruler while the Cinder Dragon still owns the mountain's heart." << endl;
        addQuest("Defeat the Cinder Dragon");
    }
}



bool isImportantLorePlaceType(const char biomeType) const {
    return biomeType == 'S' || biomeType == 'G' || biomeType == 'O' || biomeType == 'H' ||
           biomeType == 'T' || biomeType == 'Y' || biomeType == 'L';
}

string getImportantLoreGoalName(const char biomeType) const {
    if (biomeType == 'S') return "Dune Maw";
    if (biomeType == 'G') return "Crystal Matriarch";
    if (biomeType == 'O') return "Ancient Ent";
    if (biomeType == 'H') return "Marionette Coven";
    if (biomeType == 'T') return "Claim The Throne";
    if (biomeType == 'Y') return "Tide Leviathan";
    if (biomeType == 'L') return "Cinder Dragon";
    return "Unknown Lore Event";
}

bool importantLoreEventIsUnfinished(const char biomeType) const {
    if (biomeType == 'S') return !hasStoryFlag("Dune Maw Defeated");
    if (biomeType == 'G') return !hasStoryFlag("Crystal Matriarch Defeated");
    if (biomeType == 'O') return !hasStoryFlag("Ancient Ent Defeated");
    if (biomeType == 'H') return !hasStoryFlag("Marionette Coven Defeated");
    if (biomeType == 'Y') return !hasStoryFlag("Tide Leviathan Defeated");
    if (biomeType == 'L') return !dragonDefeated;

    // The Throne should not be endlessly replayable before the Dragon is dead.
    // It becomes revisit-triggerable only when the final claim is actually possible.
    if (biomeType == 'T') return dragonDefeated && !gameWon;

    return false;
}

bool currentExploredSpaceShouldRunLoreOnlyEvent() const {
    const char biomeType = getCurrentBiomeType();
    return currentSpaceHasBeenExplored() && isImportantLorePlaceType(biomeType) && importantLoreEventIsUnfinished(biomeType);
}

void describeLoreOnlyRevisit() const {
    cout << "You return to the " << getCurrentBiomeName() << "." << endl;
    cout << "This place has already given up its smaller secrets, so only the unfinished lore remains." << endl;
    cout << "You may face that path now, or leave and come back later without taking any side reward again." << endl;
    cout << "Unfinished lore path: " << getImportantLoreGoalName(getCurrentBiomeType()) << "." << endl << endl;
}

void printQuestLog() const {
    cout << endl << "===== QUEST LOG =====" << endl;

    if (activeQuests.empty()) {
        cout << "No active objectives." << endl;
    } else {
        cout << "Active Objectives:" << endl;
        for (int i = 0; i < activeQuests.size(); i++) {
            cout << "  " << i + 1 << ". " << activeQuests[i] << endl;
        }
    }

    cout << endl << "Completed Objectives:" << endl;
    if (completedQuests.empty()) {
        cout << "  None yet." << endl;
    } else {
        for (const auto& quest : completedQuests) {
            cout << "  - " << quest << endl;
        }
    }

    cout << endl << "Discovered Major Places: " << discoveredLandmarks.size() << " / 7" << endl;
    if (discoveredLandmarks.empty()) {
        cout << "  None yet." << endl;
    } else {
        for (const auto& landmark : discoveredLandmarks) {
            cout << "  - " << landmark << endl;
        }
    }

    cout << endl;
}

    void saveStringVector(ofstream& out, const vector<string>& values) const {
    out << values.size() << endl;
    for (const auto& value : values) {
        out << quoted(value) << endl;
    }
}

bool loadStringVector(ifstream& in, vector<string>& values) {
    int count = 0;
    in >> count;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    if (count < 0) {
        return false;
    }

    values.clear();
    for (int i = 0; i < count; i++) {
        string line;
        string value;
        getline(in, line);
        istringstream reader(line);
        reader >> quoted(value);
        values.emplace_back(value);
    }

    return true;
}

bool saveGame(const string& fileName = "armagesta_save.txt") const {
    ofstream out(fileName);
    if (!out) {
        cout << "Could not create save file." << endl;
        return false;
    }

    out << "ARMAGESTA_SAVE_V2" << endl;

    out << health << ' ' << maxHealth << ' ' << critChance << ' ' << accuracy << ' '
        << hardiness << ' ' << permanentDamageModifier << ' ' << permanentMomentum << ' '
        << soul << ' ' << maxSoul << ' ' << currentScrapMetal << endl;

    out << currentPosition[0] << ' ' << currentPosition[1] << endl;

    out << mainQuestStarted << ' ' << dragonDefeated << ' ' << throneClaimed << ' ' << gameWon << endl;

    saveStringVector(out, actions);
    saveStringVector(out, combatActions);
    saveStringVector(out, combatActionsDescriptions);
    saveStringVector(out, activeQuests);
    saveStringVector(out, completedQuests);
    saveStringVector(out, discoveredLandmarks);
    saveStringVector(out, storyFlags);
    saveStringVector(out, consumedMonsterSouls);

    out << exploredMap.size() << ' ' << exploredMap[0].size() << endl;
    for (const auto& row : exploredMap) {
        for (bool explored : row) {
            out << (explored ? '1' : '0');
        }
        out << endl;
    }

    cout << "Game saved to " << fileName << "." << endl;
    return true;
}

bool loadGame(const string& fileName = "armagesta_save.txt") {
    ifstream in(fileName);
    if (!in) {
        cout << "No save file found named " << fileName << "." << endl;
        return false;
    }

    string magic;
    getline(in, magic);
    const bool saveIsV1 = magic == "ARMAGESTA_SAVE_V1";
    const bool saveIsV2 = magic == "ARMAGESTA_SAVE_V2";
    if (!saveIsV1 && !saveIsV2) {
        cout << "This save file is not compatible with this version of Armagesta." << endl;
        return false;
    }

    in >> health >> maxHealth >> critChance >> accuracy
       >> hardiness >> permanentDamageModifier >> permanentMomentum
       >> soul >> maxSoul >> currentScrapMetal;

    if (currentPosition.size() < 2) {
        currentPosition = {5, 5};
    }
    in >> currentPosition[0] >> currentPosition[1];
    in >> mainQuestStarted >> dragonDefeated >> throneClaimed >> gameWon;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!loadStringVector(in, actions)) return false;
    if (!loadStringVector(in, combatActions)) return false;
    if (!loadStringVector(in, combatActionsDescriptions)) return false;
    if (!loadStringVector(in, activeQuests)) return false;
    if (!loadStringVector(in, completedQuests)) return false;
    if (!loadStringVector(in, discoveredLandmarks)) return false;
    if (!loadStringVector(in, storyFlags)) return false;
    if (saveIsV2) {
        if (!loadStringVector(in, consumedMonsterSouls)) return false;
    } else {
        consumedMonsterSouls.clear();
    }

    int rows = 0;
    int columns = 0;
    in >> rows >> columns;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    if (rows <= 0 || columns <= 0) {
        cout << "Save file has invalid explored-map size." << endl;
        return false;
    }

    exploredMap = vector<vector<bool>>(rows, vector<bool>(columns, false));
    for (int i = 0; i < rows; i++) {
        string row;
        getline(in, row);
        for (int j = 0; j < columns && j < row.size(); j++) {
            exploredMap[i][j] = row[j] == '1';
        }
    }

    normalizePlayerStateAfterLoad();

    cout << "Game loaded from " << fileName << "." << endl;
    cout << "You are at: " << getCurrentBiomeName() << endl;
    return true;
}

};

#endif //ARMAGESTA_PLAYERDATABASE_H
