//
// Created by Taylor Anderson on 6/15/26.
//


#ifndef ARMAGESTA_ENCOUNTER_H
#define ARMAGESTA_ENCOUNTER_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"

using namespace std;


class Encounter {
private:
    int health;
    string name;
    vector<string> actionNames;
    vector<int> actionChances;
    //Action types do different kinds of things, and will call a different function below depending on what they do.
protected:
    explicit Encounter(const int health, string name, vector<string> actionNames, vector<int> actionChances)
    : health(health), name(std::move(name)), actionNames(std::move(actionNames)), actionChances(std::move(actionChances)) {};
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
};

class NewEncounterTemplate : public Encounter {
private:
    NewEncounterTemplate()
    : Encounter(7, "Skeleton", {"Stab", "Clamber"}, {60, 20}) {};

    void doAction() {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        if (nameOfAction == "Action") {

        }
    }
};
#endif //ARMAGESTA_ENCOUNTER_H
