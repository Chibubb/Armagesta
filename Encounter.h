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
    vector<int> actionTypes;
    //Action types do different kinds of things, and will call a different function below depending on what they do.
protected:
    explicit Encounter(const int health, string name, vector<string> actionNames, vector<int> actionTypes) : health(health), name(std::move(name)), actionNames(std::move(actionNames)), actionTypes(std::move(actionTypes)) {};
    virtual ~Encounter() = default;
public:
    void virtual doAction() {

    };
};

class NewEncounterTemplate : public Encounter {
private:
    NewEncounterTemplate(const int health, string name, vector<string> actionNames, vector<int> actionTypes) : Encounter(health, std::move(name), std::move(actionNames), std::move(actionTypes)) {};
};
#endif //ARMAGESTA_ENCOUNTER_H
