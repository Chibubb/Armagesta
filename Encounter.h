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
#include "BackgroundMusicManager.h"

using namespace std;


class Encounter {
private:
    string name;
    vector<string> actionNames;
    vector<string> actionIntents;
    vector<int> actionChances;


    //Action types do different kinds of things, and will call a different function below depending on what they do.

public:
    explicit Encounter(const int health, string name, vector<string> actionNames, vector<int> actionChances, vector<string> Intents, Player& playerData, BackgroundMusicManager& musicManager)
    : health(health), name(std::move(name)), actionNames(std::move(actionNames)), actionChances(std::move(actionChances)), playerIP(playerData), actionIntents(std::move(Intents)), musicManager(musicManager) {};
    virtual ~Encounter() = default;

    int health;
    Player& playerIP;
    BackgroundMusicManager& musicManager;

    struct playerState {
        int temporaryDamageModifier = 0;
        int momentum = 0;
    };

    struct monsterState {
        int temporaryDamageModifier = 0;
        int danger = 0;
    };

    playerState PS;
    monsterState MS;

    int virtual getAction() {
        int x = -1;
        int totalChance = 0;
        for (const int chance : actionChances) {
            totalChance += chance;
        }
        const int choice = randomNum(1, totalChance);
        int y = 0;
        for (int i = 0; i < actionChances.size(); i++) {
            y = y + actionChances[i];
            if (choice <= y) {
                x = i;
                break;
            }
        }
        return x;
    };

    string getActionName(const int x) const {
        return actionNames[x];
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
        PS.momentum = playerIP.permanentMomentum;
        while (health > 0) {
            doAction();
            makeZeroIfNegative(playerIP.health);
            makeZeroIfNegative(health);
            cout << "You have " << playerIP.health << " Health remaining, have " << PS.momentum << " Momentum, and have " << playerIP.soul << " Souls"<< endl;
            cout << "The Enemy has " << health << " Health remaining, and " << MS.danger << " Danger" << endl << endl;
        }
        playerIP.gainSoul(1);
        musicManager.stopMusicWithFade();
        cout << "The " << name << " Died! You gained 1 Soul!"<< endl << endl;
    }

    void virtual printIntent(const int indexOfIntent) const {
        cout << actionIntents[indexOfIntent] << endl << endl;
    }



    //Possible Player Actions and what they do

    PAD doPlayerTurn_AndGetPlayerActionType(const MAD& MAD) {
        string playerAction = playerIP.getCombatAction();

        PAD PAD;
        const int roll = randomNum(1, 100);

        if (playerAction == "Slash") {

            if (roll <= 100 - playerIP.accuracy) {
                cout << "You MISSED!" << endl;
                PAD.type = "MISSED HIT";
            } else if (roll > 100 - playerIP.critChance) {
                int damageDealt = randomNum(10, 14) + playerIP.permanentDamageModifier + PS.temporaryDamageModifier - MAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                health -= damageDealt;
                cout << "CRITICAL Slash Hit!" << endl;
                cout << "You dealt " << damageDealt << " damage!" << endl;
                PAD.type = "CRITICAL HIT";
            } else {
                int damageDealt = randomNum(6, 9) + playerIP.permanentDamageModifier + PS.temporaryDamageModifier - MAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                health -= damageDealt;
                cout << "You Slashed the " << name << "!" << endl;
                cout << "You dealt " << damageDealt << " damage!" << endl;
                PAD.type = "ATTACK";
            }
        } else if (playerAction == "Eviscerate") {

            if (PS.momentum >= 5){
                PS.momentum -= 5;
                if (roll <= 90 - playerIP.accuracy) {
                    cout << "You MISSED!" << endl;
                    PAD.type = "MISSED HIT";
                } else if (roll > 100 - playerIP.critChance) {
                    int damageDealt = randomNum(25, 30) + playerIP.permanentDamageModifier + PS.temporaryDamageModifier - MAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    health -= damageDealt;
                    cout << "CRITICAL Evisceration!" << endl;
                    cout << "You dealt " << damageDealt << " damage!" << endl;
                    PAD.type = "CRITICAL HIT";
                } else {
                    int damageDealt = randomNum(17, 20) + playerIP.permanentDamageModifier + PS.temporaryDamageModifier - MAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    health -= damageDealt;
                    cout << "You Eviscerated the " << name << "!" << endl;
                    cout << "You dealt " << damageDealt << " damage!" << endl;
                    PAD.type = "ATTACK";
                }
            } else {
                cout << "You did not have the Momentum required to Eviscerate!" << endl;
                PAD.type = "NO TYPE";
            }
        } else if (playerAction == "Roll") {

            if (PS.momentum >= 1){
                if (roll < 20 * PS.momentum) {
                    PAD.defenceValue += 10000;
                    cout << "You Rolled and completely avoided the " << name << "'s attack!" << endl;
                } else {
                    cout << "Your Roll failed!" << endl;
                }
                PS.momentum -= 1;
                PAD.type = "DODGE";
            } else {
                cout << "You did not have the Momentum required to Roll!" << endl;
                PAD.type = "NO TYPE";
            }
        } else if (playerAction == "Brace") {

            int defenceGain = randomNum(4, 6) + playerIP.hardiness;
            makeZeroIfNegative(defenceGain);
            PAD.defenceValue += defenceGain;
            cout << "You Braced for impact, and blocked for " << defenceGain << " defence!" << endl;
            PAD.type = "DEFENSE";
        } else if (playerAction == "Think") {

            PS.momentum += 2;
            cout << "You took a moment to assess the situation and look for an opening, and gained 2 Momentum!" << endl;
            PAD.type = "CHARGE";
        } else if (playerAction == "Riposte") {
            if (MAD.type == "M-ATTACK" && MAD.attackSpeed != "Quick") {
                if (roll <= 100 - playerIP.accuracy) {
                    int damageDealt = randomNum(4, 7) + playerIP.permanentDamageModifier + PS.temporaryDamageModifier - MAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    health -= damageDealt;
                    int defenceGain = randomNum(2, 4) + playerIP.hardiness;
                    makeZeroIfNegative(defenceGain);
                    PAD.defenceValue += defenceGain;
                    PS.momentum += 1;
                    cout << "You Parried the " << name << "'s Attack! You dealt " << damageDealt << " damage, gained " << defenceGain << " defence, and gained 1 Momentum!" << endl;
                    PAD.type = "PARRY";
                } else {
                    cout << "You tried to Parry, but Missed!" << endl;
                    PAD.type = "MISSED HIT";
                }
            } else if (MAD.type == "M-ATTACK" && MAD.attackSpeed == "Quick") {
                cout << "You Parried, but the " << name << " Attacked too quick to Parry!" << endl;
                PAD.type = "MISSED HIT";
            }else if (MAD.type == "R-ATTACK"){
                cout << "You Parried, but the " << name << " was not using a Melee Attack!" << endl;
                PAD.type = "NO TYPE";
            } else {
                cout << "You Parried, but the " << name << " was not Attacking!" << endl;
                PAD.type = "NO TYPE";
            }
        } else if (playerAction == "Weaken") {

            if (playerIP.soul >= 2) {
                playerIP.soul -= 2;
                MS.temporaryDamageModifier -= 2;
                cout << "You spent 2 Souls and used Weaken! The " << name << " strength went down by 2!" << endl;
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Weaken, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
            }
        } else if (playerAction == "Break Sapphire Amulet") {

            cout << "The now freed souls flow into you! You have regained souls to your max potential!" << endl;
            playerIP.soul = playerIP.maxSoul;
            int damageDealt = randomNum(13, 17);
            health -= damageDealt;
            cout << "The amulet breaking activated a Soul Burst! You dealt " << damageDealt << " Magical damage to the " << name << "!" << endl;
            PAD.type = "MAGIC";
        } else if (playerAction == "Soul Burst") {

            if (playerIP.soul >= 4) {
                playerIP.soul -= 4;
                int damageDealt = randomNum(13, 17);
                health -= damageDealt;
                cout << "You spent 4 Souls and used Soul Burst! You dealt " << damageDealt << " Magical damage to the " << name << "!" << endl;
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Soul Burst, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
            }
        } else if (playerAction == "Ent Wand") {

            if (playerIP.soul >= 5) {
                playerIP.soul -= 5;
                int damageDealt = randomNum(15, 18);
                health -= damageDealt;
                PAD.defenceValue = 10;
                MS.temporaryDamageModifier -= 2;
                cout << "Roots come up all over the place! They simultaneously block the " << name << " attack and splinter its ligaments!" << endl;
                cout << "You dealt " << damageDealt << " damage to the " << name << "!" << endl;
                PAD.type = "MAGIC";
            }
        }
        return PAD;
    }

    //Possible Player Actions and what they do



};





//Kinds of Monsters

class Slime : public Encounter {
private:
public:
    explicit Slime(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(15, "Slime", {"Slap", "Goo", "Wiggle"}, {60, 20, 20}
        , {"The Slime starts to bounce around...", "The Slime undulates and vibrates...", "The Slime sits and starts to shift and jiggle..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Slap") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type != "CRITICAL HIT" && PAD.type != "PARRY") {
                    int damageDealt = randomNum(3, 5) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Slime slapped you, and did " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "PARRY") {
                    int damageDealt = randomNum(3, 5) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    MS.temporaryDamageModifier -= 1;
                    cout << "Your Parry Hit split a part off the Slime! It's attack has Weakened!" << endl;
                    cout << "The Slime has nonetheless slapped you, and done " << damageDealt << " damage!" << endl;
                } else {
                    cout << "Your Critical Hit canceled the enemies Attack!" << endl;
                }
            }else if (nameOfAction == "Goo") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                PS.temporaryDamageModifier -= 2;
                if (PAD.type == "MISSED HIT") {
                    PS.temporaryDamageModifier -= 2;
                }
                cout << "The Slime threw Goo at you! Your attack has decreased..." << endl;
            }else if (nameOfAction == "Wiggle") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                cout << "The Slime does practically nothing, except sit there and Wiggle!" << endl;
            }
        }
    }
};

class Skeleton : public Encounter {
private:
public:
    explicit Skeleton(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(35, "Skeleton", {"Stab", "Clamber", "Bow", "Put Back Together"}, {40, 15, 20, 20}
        ,{"The Skeleton lunges at you with a knife...", "The Skeleton begins to jitter and shake...",
            "The Skeleton draws its Bow...", "The Skeleton scrambles to pick up its detached pieces..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Stab") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type == "CHARGE") {
                    int damageDealt = randomNum(20, 24) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Skeleton takes advantage of your vulnerable state and slices at you three times, dealing " << damageDealt << " damage!" << endl;
                } else {
                    int damageDealt = randomNum(9, 11) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                }
            } else if (nameOfAction == "Clamber") {
                MAD MAD;
                MAD.defenceValue = randomNum(10, 12);
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type == "MISSED HIT" || PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT") {
                    cout << "You fell for the Skeleton's trap!" << endl;
                    int damageDealt = randomNum(15, 17) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 1;
                    cout << "It grabs your incoming attack, dislodges your elbow, and bites the SHIT out of your shoulder!" << endl;
                } else {
                    cout << "You avoided the Skeleton's lure..." << endl;
                }
            } else if (nameOfAction == "Bow") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type == "CHARGE") {
                    int damageDealt = randomNum(15, 17) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 3;
                    PS.momentum = 0;
                    cout << "The Skeleton takes advantage of your vulnerable position and shoots an arrow straight to your lung, weakening your breathing, and dealing " << damageDealt << " damage!" << endl;
                } else {
                    int n = randomNum(1, 10);
                    if (n <= 7 || PAD.type == "DEFENSE") {
                        int damageDealt = randomNum(7, 9) - PAD.defenceValue;
                        makeZeroIfNegative(damageDealt);
                        playerIP.health -= damageDealt;
                        PS.temporaryDamageModifier -= 1;
                        cout << "The arrow hits your kneecap!" << endl;
                    } else {
                        cout << "The arrow missed you!" << endl;
                    }
                }
            } else if (nameOfAction == "Put Back Together") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your attack was powerful enough that the Skeleton was interrupted while trying to put itself back together!" << endl;
                    MS.temporaryDamageModifier -= 2;
                } else if (PAD.type == "MAGIC") {
                    cout << "The pure magic from what you did interrupted the Skeleton putting itself back together!" << endl;
                    MS.temporaryDamageModifier -= 1;
                } else {
                    cout << "The Skeleton picks up some of its pieces, CHINKING them back onto its body" << endl;
                    health += 10;
                }
            }
        }
    }
};

//Kinds of Monsters





// New Encounter Template

class NewEncounterTemplate : public Encounter {
private:
public:
    explicit NewEncounterTemplate(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(7, "Skeleton", {"Stab", "Clamber"}, {60, 20}
        ,{"", ""}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Action") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);

            }
        }
    }
};

// New Encounter Template






#endif //ARMAGESTA_ENCOUNTER_H
