//
// Created by Taylor Anderson on 6/15/26.
//


#ifndef ARMAGESTA_ENCOUNTER_H
#define ARMAGESTA_ENCOUNTER_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include <vector>
#include "PlayerDatabase.h"

using namespace std;


class Encounter {
private:
    int health;
    string name;
    vector<string> actionNames;
    vector<int> actionChances;
    Player& playerIP;
    //Action types do different kinds of things, and will call a different function below depending on what they do.
protected:
    explicit Encounter(const int health, string name, vector<string> actionNames, vector<int> actionChances, Player& playerData)
    : health(health), name(std::move(name)), actionNames(std::move(actionNames)), actionChances(std::move(actionChances)), playerIP(playerData) {};
    virtual ~Encounter() = default;
public:
    int virtual getAction() {
        int x = -1;
        int totalChance = 0;
        for (const int chance : actionChances) {
            totalChance += chance;
        }
        const int choice = randomNum(1, totalChance);
        int y = actionChances.at(0);
        for (int i = 0; i < actionChances.size(); i++) {
            if (choice <= y) {
                x = i;
            } else {
                y = y + actionChances.at(i + 1);
            }
        }
        return x;
    };
    string getActionName(const int x) const {
        return actionNames.at(x);
    }
    string getName() const {
        return name;
    }
    int getHealth() const {
        return health;
    }
    Player& getPlayerIP() const {
        return playerIP;
    }

    void virtual doAction();

};

class Slime : public Encounter {
private:
public:
    explicit Slime(Player& playerData)
    : Encounter(15, "Slime", {"Slap", "Goo"}, {80, 20}, playerData) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        if (nameOfAction == "Slap") {

        }else if (nameOfAction == "Goo") {

        }
    }
};



// New Encounter Template

class NewEncounterTemplate : public Encounter {
private:
public:
    explicit NewEncounterTemplate(Player& playerData)
    : Encounter(7, "Skeleton", {"Stab", "Clamber"}, {60, 20}, playerData) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        if (nameOfAction == "Action") {

        }
    }
};

// New Encounter Template


#endif //ARMAGESTA_ENCOUNTER_H
