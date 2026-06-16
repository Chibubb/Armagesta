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

using namespace std;


class Encounter {
private:
    int health;
    string name;
    vector<string> actionNames;
    vector<int> actionChances;
    //Action types do different kinds of things, and will call a different function below depending on what they do.
protected:
    explicit Encounter(const int health, string name, vector<string> actionNames, vector<int> actionChances, shared_ptr<int> playerData)
    : health(health), name(std::move(name)), actionNames(std::move(actionNames)), actionChances(std::move(actionChances)), playerIP(std::move(playerData)) {};
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
    string getActionName(const int x) {
        return actionNames.at(x);
    }
    string getName() {
        return name;
    }
    int getHealth() const {
        return health;
    }
    shared_ptr<int>& getPlayerIP() {
        return playerIP;
    }
    shared_ptr<int> playerIP;
};

class Slime : public Encounter {
private:
public:
    explicit Slime(shared_ptr<int> playerData)
    : Encounter(15, "Slime", {"Slap", "Goo"}, {80, 20}, playerData) {};

    void virtual doAction() {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        if (nameOfAction == "Slap") {

        }else if (nameOfAction == "Goo") {

        }
    }
};

class NewEncounterTemplate : public Encounter {
private:
public:
    explicit NewEncounterTemplate(shared_ptr<int> playerData)
    : Encounter(7, "Skeleton", {"Stab", "Clamber"}, {60, 20}, playerData) {};

    void virtual doAction() {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        if (nameOfAction == "Action") {

        }
    }
};
#endif //ARMAGESTA_ENCOUNTER_H
