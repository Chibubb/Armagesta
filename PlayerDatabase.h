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

using namespace std;

class Player {
private:
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
    int maxHealth = 100;
    int mana = 10;
    int maxSoul = 10;
    vector<int> currentPosition = {5, 5};

public:

    vector<string> actions = {"Move"};

    Player() = default;

    void heal(const int healAmount) {
        health += healAmount;
    }

    char move(const string& direction) {

        if (direction == "west" || direction == "West") {
            if (currentPosition[1] != 0) {
                currentPosition[1] -= 1;
            }
        }else if (direction == "east" || direction == "East") {
            if (currentPosition[1] != 10) {
                currentPosition[1] += 1;
            }
        }else if (direction == "north" || direction == "North") {
            if (currentPosition[0] != 0) {
                currentPosition[0] -= 1;
            }
        }else if (direction == "south" || direction == "South") {
            if (currentPosition[0] != 10) {
                currentPosition[0] += 1;
            }
        }
        return map[currentPosition[0]][currentPosition[1]];
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
            cout << i << ": " << actions[i] << "  ";
        }
        cout << endl << endl;
    }

    string getAction() const {
        bool x = true;
        string chosenAction;
        while (x == true) {
            cin >> chosenAction;
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

};

#endif //ARMAGESTA_PLAYERDATABASE_H
