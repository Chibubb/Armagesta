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
    vector<string> actionIntents;
    vector<int> actionChances;



    //Action types do different kinds of things, and will call a different function below depending on what they do.

public:
    explicit Encounter(const int health, string name, vector<string> actionNames, vector<int> actionChances, vector<string> Intents, Player& playerData)
    : health(health), name(std::move(name)), actionNames(std::move(actionNames)), actionChances(std::move(actionChances)), playerIP(playerData), actionIntents(std::move(Intents)) {};
    virtual ~Encounter() = default;

    Player& playerIP;

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

    void virtual doAction() = 0;

    void haveCombat() {
        while (health > 0) {
            doAction();
            cout << "You have " << playerIP.health << " remaining" << endl;
            cout << "The Enemy has " << health << " remaining" << endl;
        }
        cout << "The Monster Died!" << endl << endl;
    }

    void printIntent(const int indexOfIntent) const {
        cout << actionIntents.at(indexOfIntent) << endl;
    }



    //Possible Player Actions and what they do

    string doPlayerTurn_AndGetPlayerActionType() {
        const string playerAction = playerIP.getCombatAction();

        const int roll = randomNum(1, 100);

        if (playerAction == "Slash") {

            if (roll < 100 - playerIP.accuracy) {
                cout << "You MISSED!" << endl;
                return "MISSED HIT";
            } else if (roll > 100 - playerIP.critChance) {
                int damageDealt = randomNum(10, 14) + playerIP.permanentDamageModifier + playerIP.temporaryDamageModifier;
                makeZeroIfNegative(damageDealt);
                health -= damageDealt;
                cout << "CRITICAL Slash Hit!" << endl;
                cout << "You dealt " << damageDealt << " damage!" << endl;
                return "CRITICAL HIT";
            } else {
                int damageDealt = randomNum(6, 9) + playerIP.permanentDamageModifier + playerIP.temporaryDamageModifier;
                makeZeroIfNegative(damageDealt);
                health -= damageDealt;
                cout << "You Slashed the Enemy" << endl;
                cout << "You dealt " << damageDealt << " damage!" << endl;
            }

            return "ATTACK";
        }

    }

    //Possible Player Actions and what they do



};





//Kinds of Monsters

class Slime : public Encounter {
private:
public:
    explicit Slime(Player& playerData)
    : Encounter(15, "Slime", {"Slap", "Goo"}, {80, 20}
        , {"", ""}, playerData) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        printIntent(x);

        string playerActionType = doPlayerTurn_AndGetPlayerActionType();

        if (nameOfAction == "Slap") {
            if (playerActionType != "CRITICAL HIT") {
                int damageDealt = randomNum(3, 5);
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                cout << "The Slime did " << damageDealt << " damage!" << endl;
            } else {
                cout << "Your Critical Hit canceled the enemies Attack!" << endl;
            }
        }else if (nameOfAction == "Goo") {
            playerIP.temporaryDamageModifier -= 2;
            if (playerActionType == "MISSED HIT") {
                playerIP.temporaryDamageModifier -= 2;
            }
            cout << "The Slime threw Goo at you! Your attack has decreased..." << endl;
        }
    }
};

//Kinds of Monsters





// New Encounter Template

class NewEncounterTemplate : public Encounter {
private:
public:
    explicit NewEncounterTemplate(Player& playerData)
    : Encounter(7, "Skeleton", {"Stab", "Clamber"}, {60, 20}
        ,{"", ""}, playerData) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        printIntent(x);

        string playerActionType = doPlayerTurn_AndGetPlayerActionType();

        if (nameOfAction == "Action") {

        }
    }
};

// New Encounter Template






#endif //ARMAGESTA_ENCOUNTER_H
