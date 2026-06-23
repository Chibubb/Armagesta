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
#include "MonsterCalling.h"
#include "BackgroundMusicManager.h"
#include "PlayerDatabase.h"

using namespace std;



class BiomeEventDatabase {
protected:
    BackgroundMusicManager& musicManager;
    Player& playerIP;
    explicit BiomeEventDatabase(Player& playerIP, BackgroundMusicManager& musicManager) : playerIP(playerIP), musicManager(musicManager) {};
    virtual ~BiomeEventDatabase() = default;

    int virtual getChoice() = 0;
    void virtual doEvent() = 0;
    void virtual ResolveChoice() = 0;

};

class Forest : public BiomeEventDatabase {
private:
    vector<int> eventChances = {60, 40, 5, 5};
    vector<string> eventNames = {"Bright Forest", "Dark Brush", "Hidden Cathedral", "Grand Tree"};
    vector<string> eventFlavorText = {
        "Light shines through the trees, illuminating the leafy forest floor",
        "Whispers flow through the trees, making it clear you are not alone",
        "A tall Cathedral hidden in he trees stands before you, covered in vines, and clearly deteriorated over many decades",
        "A tree is rooted in front of you, standing at least three times as tall as any other that you have seen in the forest"
    };
    vector<vector<string>> eventChoices = {
        {"Rest", "Scavenge"},
        {"Confront", "Run Away"},
        {"Explore the Basement", "Appreciate the Stained Glass Artwork", "Pray at the Altar", "Move On"},
        {"Talk to the Tree", "Climb the Tree", "Carve into the Tree", "Move On"}
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
            bool firstTime = true;
            while (true) {
                getline(cin, triedChoice);
                int k = 0;
                for (k = 0; k < eventChoices[REI].size(); k++) {
                    if (eventChoices[REI][k] == triedChoice) {
                        eventChoiceCorrectlyChosen = true;
                        ChosenChoice = k;
                    }
                }

                if (eventChoiceCorrectlyChosen == true) {
                    break;
                }

                if (firstTime == false) {
                    cout << "Invalid input, try again..." << endl;
                }

                firstTime = false;

            }
        } else {
            ChosenChoice = 0;
        }

        ChoiceName = eventChoices[REI][ChosenChoice];

        ResolveChoice();

    }

    void ResolveChoice() override {
        if (ChoiceName == "Rest") {
            playerIP.heal(2);
            cout << "You have rested back 2 Health!" << endl;
        } else if (ChoiceName == "Confront") {
            cout << "A Slime appears!" << endl;
            encounterSlime(playerIP, musicManager);
        } else if (ChoiceName == "Run Away") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                playerIP.health -= 10;
                cout << "You successfully get away, but are wounded in your escape" << endl;
            } else {
                cout << "You got away without getting wounded!" << endl;
            }
        } else if (ChoiceName == "Scavenge") {
            const int n = randomNum(1, 10);
            if (n <= 6) {
                cout << "You have found nothing..." << endl;
            } else if (n <= 8) {
                cout << "You have found a piece of Scrap Metal" << endl;
                playerIP.currentScrapMetal += 1;
            } else if (n <= 9) {
                cout << "You have found a juicy Plum! You have healed 10 Health!" << endl;
                playerIP.heal(10);
            } else if (n <= 10) {
                cout << "You have found a Hearty Salmon! After some light cooking, it has become quite the meal!" << endl;
                playerIP.maxHealth += 5;
            }
        } else if (ChoiceName == "Explore the Basement") {
            const int n = randomNum(1, 10);
            if (n <= 7) {
                cout << "A Skeleton appears!" << endl;
                encounterSkeleton(playerIP, musicManager);
            }
            if (n <= 2) {
                cout << "Another Skeleton appears!" << endl;
                encounterSkeleton(playerIP, musicManager);
            }
            cout << "You find a strange amulet, with a silver rim, and a central Sapphire" << endl;
            playerIP.maxSoul += 1;
            playerIP.combatActions.emplace_back("Break Sapphire Amulet");
            playerIP.combatActionsDescriptions.emplace_back("It looks like tiny wisps flow and spin inside the Sapphire, I wonder what breaking it would do?");
        } else if (ChoiceName == "Appreciate the Stained Glass Artwork") {
            cout << "The stained glass artwork fills you with determination! You feel a power flow within you, and a pane of glass depicting a floral sword shatters" << endl;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Pray at the Altar") {
            const int n = randomNum(1, 10);
            cout << "There is a large brown book on the alter..." << endl;
            if (n <= 7) {
                cout << "Out of the book comes a warm familiar soul, you absorb it. You have gained 5 souls, and your soul capacity feels bigger!" << endl;
                playerIP.maxSoul += 2;
                playerIP.gainSoul(5);
            } else {
                cout << "A large soul with demonic energy radiating from it emerges from the book, it pounces at you, dissipating into your chest..." << endl;
                playerIP.maxSoul += 2;
                playerIP.maxHealth -= 5;
                if (playerIP.maxHealth > playerIP.health) {
                    playerIP.health = playerIP.maxHealth;
                }
            }
        } else if (ChoiceName == "Move On") {
            cout << "Best leave that for another day..." << endl;
            cout << "You feel slightly better knowing that this is YOUR choice";
            playerIP.heal(3);
        } else if (ChoiceName == "Talk to the Tree") {
            const int n = randomNum(1, 10);
            cout << "The Tree talks back!" << endl;
            if (n <= 5) {
                cout << "Although the tree doesn't seem to want to talk much, but it does drop a couple apple's for you!" << endl;
                playerIP.heal(5);
            } else if (n <= 7) {
                cout << "The Tree seems irrationally angry, it bumps you away like a bug" << endl;
                playerIP.health -= 5;
            } else if (n <= 10) {
                cout << "The Tree describes itself as an Ent, and says it has been here for centuries. It gives you a Hardwood stick from the top of its head and bids you off..." << endl;
                playerIP.combatActions.emplace_back("Ent Wand");
                playerIP.combatActionsDescriptions.emplace_back("This wand is a classic Ent trick, bring up roots to stab the enemy, as well as binding its movement, this requires a large amount of souls");
            }
        } else if (ChoiceName == "Climb the Tree") {
            cout << "How many meters do you want to climb?" << endl;
            int x;
            cin >> x;
            while (x <= 0 || x > 100) {
                cout << "You can't even climb that high..." << endl;
                cin >> x;
            }
            const int n = randomNum(1, x);
            if (n <= 10) {
                cout << "You found an orange!" << endl;
                playerIP.heal(5);
            } else if (n <= 30) {
                cout << "You found a large Blood Orange!" << endl;
                playerIP.maxHealth += 5;
                playerIP.heal(15);
            } else if (n <= 90) {
                cout << "You fell quite high from the Tree!" << endl;
                const int y = randomNum(1, 30);
                cout << "You took " << y << " damage!" << endl;
            } else if (n <= 100) {
                cout << "At the top of the tree, it feels like you can see the entire world!" << endl;
                playerIP.printMap();
            }
        } else if (ChoiceName == "Carve into the Tree") {
            cout << "The Tree awakens, and seems very angry!" << endl;
            encounterEnt(playerIP, musicManager);
            cout << "From the corpse of the ent is a large piece of unbreakable bark, you grab it, and it melds to your arm...";
            playerIP.hardiness += 5;
            playerIP.heal(30);
        }
    }

public:
    explicit Forest(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
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
    explicit NEWBIOMENAME(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = NEWBIOMENAME::getChoice();
        NEWBIOMENAME::doEvent();
    }
};

// Template for Biomes


#endif //ARMAGESTA_EVENTDATABASE_H
