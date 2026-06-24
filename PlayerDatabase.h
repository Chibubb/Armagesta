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
    vector<string> combatActions = {"Slash", "Eviscerate", "Roll", "Brace", "Think", "Riposte"
        , "Weaken", "Soul Burst"};
    vector<string> combatActionsDescriptions = {"Use your sword on the enemy, like you have done so many times before"
        , "Make the enemy's innards outards, requires a large amount of Momentum"
        , "Try to dodge you enemy's attack by rolling, requires miniscule Momentum"
        , "Block incoming damage using known battle stances"
        , "Take a moment of respite, and assess the situation around you, gains you Momentum"
        , "Parry an enemy who is choosing to attack you, dealing damage, acquiring defense, and gaining Momentum"
        , "Caste a weakening curse on your enemy, immediately enacting bodily distrophy on them, requires a small amount of souls"
        , "Unleash the power of stolen souls on the currently living enemy in front of you, dealing a large amount of damage, requires a decent amount of souls"};

    Player() {
        if (exploredMap.empty()) {
            exploredMap = vector<vector<bool>>(map.size(), vector<bool>(map[0].size(), false));
        }

        markCurrentSpaceExplored();

        unlockWorldAction("Map");
        unlockWorldAction("Quest Log");
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
        cout << "You already know " << actionName << "." << endl;
        return;
    }

    combatActions.emplace_back(actionName);
    combatActionsDescriptions.emplace_back(description);
    cout << "NEW COMBAT ACTION UNLOCKED: " << actionName << endl;
    cout << description << endl;
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
    cout << endl << "NEW OBJECTIVE: " << questName << endl;
}

void completeQuest(const string& questName) {
    if (hasCompletedQuest(questName)) {
        return;
    }

    eraseFromVector(activeQuests, questName);
    completedQuests.emplace_back(questName);
    cout << endl << "OBJECTIVE COMPLETE: " << questName << endl;
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
    cout << endl << "LANDMARK DISCOVERED: " << landmarkName << endl;

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
        cout << endl;
        cout << "The Throne accepts you." << endl;
        cout << "The Dragon is dead, the seven places have spoken, and Armagesta finally has a will again." << endl;
    } else {
        cout << endl;
        cout << "The Throne burns cold beneath you. It will not fully accept a ruler while the Cinder Dragon still owns the mountain's heart." << endl;
        addQuest("Defeat the Cinder Dragon");
    }
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

    out << "ARMAGESTA_SAVE_V1" << endl;

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
    if (magic != "ARMAGESTA_SAVE_V1") {
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

    cout << "Game loaded from " << fileName << "." << endl;
    cout << "You are at: " << getCurrentBiomeName() << endl;
    return true;
}

};

#endif //ARMAGESTA_PLAYERDATABASE_H
