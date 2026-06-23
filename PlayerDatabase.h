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

using namespace std;

class Player {
private:
public:

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

    vector<string> actions = {"Move", "Self Assess", "Understand Powers"};
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

    Player() = default;

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
            cin >> chosenDirection;
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
        for (int i = 0; i < map.size(); i++) {
            for (int j = 0; j < map[i].size(); j++) {
                cout << map[i][j] << " ";
            }
            cout << endl;
        }
    }

};

#endif //ARMAGESTA_PLAYERDATABASE_H
