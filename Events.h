//
// Created by Taylor Anderson on 6/16/26.
//

#ifndef ARMAGESTA_EVENTDATABASE_H
#define ARMAGESTA_EVENTDATABASE_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include <vector>

#include "PlayerDatabase.h"

using namespace std;



class BiomeEventDatabase {
protected:
    Player& playerIP;
    explicit BiomeEventDatabase(Player& playerIP) : playerIP(playerIP) {};
    virtual ~BiomeEventDatabase() = default;

    int virtual getChoice() = 0;
    void virtual doEvent() = 0;
    void virtual ResolveChoice() = 0;
};

class Forest : public BiomeEventDatabase {
private:
    vector<int> eventChances = {70, 30};
    vector<string> eventNames = {"Bright Forest", "Dark Forest"};
    vector<string> eventFlavorText = {
        "Light shines through the trees, illuminating the leafy forest floor",
        "Whispers flow through the trees, making it clear you are not alone"
    };
    vector<vector<string>> eventChoices = {
        {"Rest", "Scavenge"},
        {"Confront", "Run Away"}
    };

    string ChoiceName;

    int REI = -1;

protected:
    int getChoice() override {
        int totalChance = 0;
        for (auto i : eventChances) {
            totalChance += i;
        }
        int y = randomNum(1, totalChance);
        int cumulativeChance = 0;
        for (int i = 0; i < eventChances.size(); i++) {
            cumulativeChance += eventChances[i];
            if (cumulativeChance >= y) {
                return i;
            }
        }
        return -1;
    }

    void doEvent() override{
        int ChosenChoice = 0;
        cout << eventNames[REI] << endl;
        cout << eventFlavorText[REI] << endl << endl;
        if (eventChoices[REI].size() != 1) {
            cout << "Choose one:" << endl;
            int j = 1;
            for (const auto& i : eventChoices[REI]) {
                cout << j << ": " << i << endl;
                j += 1;
            }
            cout << endl;
            bool eventChoiceCorrectlyChosen = false;
            string triedChoice;
            while (eventChoiceCorrectlyChosen == false) {
                cin >> triedChoice;
                int k = 0;
                for (k = 0; k < eventChoices[REI].size(); k++) {
                    if (eventChoices[REI][k] == triedChoice) {
                        eventChoiceCorrectlyChosen = true;
                    }
                }
                if (eventChoiceCorrectlyChosen == false) {
                    cout << "Invalid input, try again..." << endl;
                } else {
                    ChosenChoice = k + 1;
                }
            }
        } else {
            ChosenChoice = 1;
        }
        ChoiceName = eventChoices[REI][ChosenChoice - 1];

        ResolveChoice();

    }

    void ResolveChoice() override {
        if (ChoiceName == "Rest") {
            playerIP.heal(2);
        }
    }

public:
    explicit Forest(Player& PlayerIP) : BiomeEventDatabase(PlayerIP) {
        REI = Forest::getChoice();
        Forest::doEvent();
    }
};

// Template for Biomes

class NEWBIOMENAME : public BiomeEventDatabase {
private:
    vector<int> eventChances = {70, 30};
    vector<string> eventNames = {"EVENT 1", "EVENT 2"};
    vector<string> eventFlavorText = {
        "EVENT 1 FLAVOR TEXT",
        "EVENT 2 FLAVOR TEXT"
    };
    vector<vector<string>> eventChoices = {
        {"CHOICE 1", "CHOICE 2"},
        {"CHOICE 1", "CHOICE 2"}
    };

    string ChoiceName;

    int REI = -1;

protected:
    int getChoice() override {
        int totalChance = 0;
        for (auto i : eventChances) {
            totalChance += i;
        }
        int y = randomNum(1, totalChance);
        int cumulativeChance = 0;
        for (int i = 0; i < eventChances.size(); i++) {
            cumulativeChance += eventChances[i];
            if (cumulativeChance >= y) {
                return i;
            }
        }
        return -1;
    }

    void doEvent() override{
        int ChosenChoice = 0;
        cout << eventNames[REI] << endl;
        cout << eventFlavorText[REI] << endl << endl;
        if (eventChoices[REI].size() != 1) {
            cout << "Choose one:" << endl;
            int j = 1;
            for (const auto& i : eventChoices[REI]) {
                cout << j << ": " << i << endl;
                j += 1;
            }
            cout << endl;
            int z = 0;
            while (z < 1 || z > eventChoices[REI].size()) {
                cin >> z;
                if (z < 1 || z > eventChoices[REI].size()) {
                    cout << endl << "Invalid Input, Try Again" << endl;
                    continue;
                }
                ChosenChoice = z;
            }
        } else {
            ChosenChoice = 1;
        }
        ChoiceName = eventChoices[REI][ChosenChoice - 1];

        ResolveChoice();

    }

    void ResolveChoice() override {
        if (ChoiceName == "Rest") {

        }
    }

public:
    explicit NEWBIOMENAME(Player& PlayerIP) : BiomeEventDatabase(PlayerIP) {
        REI = NEWBIOMENAME::getChoice();
        NEWBIOMENAME::doEvent();
    }
};

// Template for Biomes


#endif //ARMAGESTA_EVENTDATABASE_H
