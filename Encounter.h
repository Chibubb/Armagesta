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

    void printCombatStatus() const {
        cout << endl;
        cout << "===== COMBAT STATUS =====" << endl;
        cout << "You: " << playerIP.health << " / " << playerIP.maxHealth << " Health"
             << " | Souls: " << playerIP.soul << " / " << playerIP.maxSoul
             << " | Momentum: " << PS.momentum << endl;

        cout << getName() << ": " << health << " Health"
             << " | Danger: " << MS.danger << endl;

        if (PS.temporaryDamageModifier != 0 || MS.temporaryDamageModifier != 0) {
            cout << "Temporary Modifiers:" << endl;
            if (PS.temporaryDamageModifier != 0) {
                cout << "  Your temporary damage modifier: " << PS.temporaryDamageModifier << endl;
            }
            if (MS.temporaryDamageModifier != 0) {
                cout << "  Enemy temporary damage modifier: " << MS.temporaryDamageModifier << endl;
            }
        }

        cout << "=========================\n" << endl;
    }

    void haveCombat() {
        PS.momentum = playerIP.permanentMomentum;

        cout << endl << "Combat begins against the " << getName() << "!" << endl;
        printCombatStatus();

        while (health > 0 && playerIP.health > 0) {
            doAction();
            makeZeroIfNegative(playerIP.health);
            makeZeroIfNegative(health);

            printCombatStatus();
        }

        musicManager.stopMusicWithFade();

        if (playerIP.health <= 0) {
            cout << "The " << getName() << " watches you fall." << endl << endl;
            return;
        }

        playerIP.gainSoul(1);
        cout << "The " << getName() << " died! You gained 1 Soul!" << endl << endl;
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
                if (roll > 100 - playerIP.accuracy) {
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
        } else if (playerAction == "Sandstep") {

            if (playerIP.soul >= 2) {
                playerIP.soul -= 2;
                PAD.defenceValue += 9 + PS.momentum;
                PS.momentum += 2;
                cout << "You spend 2 Souls and step with the memory of dunes. Your body slips sideways before danger fully arrives." << endl;
                cout << "You gained 2 Momentum and raised your defense for this turn!" << endl;
                PAD.type = "DODGE";
            } else {
                cout << "You tried to use Sandstep, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
            }

        } else if (playerAction == "Crystal Ward") {

            if (playerIP.soul >= 3) {
                playerIP.soul -= 3;
                PAD.defenceValue += 15 + playerIP.hardiness;
                cout << "You spend 3 Souls and a clear crystal wall snaps into existence in front of you." << endl;
                if (MAD.type == "M-ATTACK" || MAD.type == "R-ATTACK") {
                    int damageDealt = randomNum(4, 8);
                    health -= damageDealt;
                    cout << "The enemy's attack rings against the ward and sends broken light back at it for " << damageDealt << " damage!" << endl;
                }
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Crystal Ward, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
            }

        } else if (playerAction == "Redwood Oath") {

            if (playerIP.soul >= 5) {
                playerIP.soul -= 5;
                int healAmount = randomNum(7, 11);
                playerIP.heal(healAmount);
                PAD.defenceValue += 12;
                PS.temporaryDamageModifier += 2;
                MS.temporaryDamageModifier -= 1;
                cout << "You spend 5 Souls and swear the old oath of bark and blood." << endl;
                cout << "Roots brace your stance, heal you for " << healAmount << ", and sharpen your next attacks." << endl;
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Redwood Oath, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
            }

        } else if (playerAction == "Bogblood Hex") {

            if (playerIP.health > 6) {
                playerIP.health -= 6;
                int damageDealt = randomNum(11, 16) + playerIP.permanentDamageModifier;
                health -= damageDealt;
                MS.temporaryDamageModifier -= 3;
                playerIP.gainSoul(1);
                cout << "You cut your palm and let swamp-dark blood write a curse in the air." << endl;
                cout << "You lose 6 Health, deal " << damageDealt << " hex damage, weaken the enemy, and regain 1 Soul." << endl;
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Bogblood Hex, but you were too wounded to pay its blood price!" << endl;
                PAD.type = "NO TYPE";
            }

        } else if (playerAction == "Royal Decree") {

            if (playerIP.soul >= 3 && PS.momentum >= 2) {
                playerIP.soul -= 3;
                PS.momentum -= 2;
                cout << "You spend 3 Souls and 2 Momentum. Your voice becomes older than your body." << endl;
                if (MAD.type == "M-ATTACK" || MAD.type == "R-ATTACK") {
                    PAD.defenceValue += 10000;
                    MS.danger -= 2;
                    makeZeroIfNegative(MS.danger);
                    cout << "By Royal Decree, the enemy's attack is forbidden from landing." << endl;
                } else {
                    int damageDealt = randomNum(7, 10);
                    health -= damageDealt;
                    MS.temporaryDamageModifier -= 1;
                    cout << "The decree finds no attack to stop, so it condemns the enemy's will for " << damageDealt << " damage instead." << endl;
                }
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Royal Decree, but you lacked either 3 Souls or 2 Momentum!" << endl;
                PAD.type = "NO TYPE";
            }

        } else if (playerAction == "Tidecall") {

            if (playerIP.soul >= 4) {
                playerIP.soul -= 4;
                int damageDealt = randomNum(10, 15);
                health -= damageDealt;
                PAD.defenceValue += 7;
                if (MAD.type == "R-ATTACK") {
                    PAD.defenceValue += 8;
                    cout << "The tide rises especially high against the incoming ranged attack." << endl;
                }
                MS.danger -= 1;
                makeZeroIfNegative(MS.danger);
                cout << "You spend 4 Souls and call a cold tide through the battlefield, dealing " << damageDealt << " damage." << endl;
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Tidecall, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
            }

        } else if (playerAction == "Dragonfire") {

            if (playerIP.soul >= 8) {
                playerIP.soul -= 8;
                int damageDealt = randomNum(26, 34) + playerIP.permanentDamageModifier;
                health -= damageDealt;
                playerIP.health -= 5;
                MS.danger += 1;
                cout << "You spend 8 Souls and breathe a stolen age of fire." << endl;
                cout << "You deal " << damageDealt << " dragonfire damage, but the heat burns you for 5 Health as well." << endl;
                PAD.type = "MAGIC";
            } else {
                cout << "You tried to use Dragonfire, but didn't have enough Souls!" << endl;
                PAD.type = "NO TYPE";
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
                    int damageDealt = randomNum(3, 5) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Slime slapped you, and did " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "PARRY") {
                    int damageDealt = randomNum(3, 5) + MS.temporaryDamageModifier - PAD.defenceValue;
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
                    int damageDealt = randomNum(20, 24) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Skeleton takes advantage of your vulnerable state and slices at you three times, dealing " << damageDealt << " damage!" << endl;
                } else {
                    int damageDealt = randomNum(9, 11) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                }
            } else if (nameOfAction == "Clamber") {
                MAD MAD;
                MAD.defenceValue = randomNum(10, 12);
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type == "MISSED HIT" || PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT") {
                    cout << "You fell for the Skeleton's trap!" << endl;
                    int damageDealt = randomNum(15, 17) + MS.temporaryDamageModifier - PAD.defenceValue;
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
                    int damageDealt = randomNum(15, 17) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 3;
                    PS.momentum = 0;
                    cout << "The Skeleton takes advantage of your vulnerable position and shoots an arrow straight to your lung, weakening your breathing, and dealing " << damageDealt << " damage!" << endl;
                } else {
                    int n = randomNum(1, 10);
                    if (n <= 7 || PAD.type == "DEFENSE") {
                        int damageDealt = randomNum(7, 9) + MS.temporaryDamageModifier - PAD.defenceValue;
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

class ThornImp : public Encounter {
private:
    int thorns = 0;
public:
    explicit ThornImp(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(24, "Thorn Imp", {"Needle Jab", "Raise Thorns", "Vine Snare", "Cackle"}, {35, 25, 25, 15}
        , {"The Thorn Imp lowers its thorny shoulder and darts toward you...",
           "The Thorn Imp digs its claws into the dirt, pulling roots up around itself...",
           "The Thorn Imp twirls a hooked vine above its head...",
           "The Thorn Imp points at your weapon and starts laughing..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Needle Jab") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit knocks the Thorn Imp out of its jab!" << endl;
                } else if (PAD.type == "PARRY") {
                    cout << "The jab was too quick to cleanly parry, but your weapon clips its thorny arm!" << endl;
                    int damageDealt = randomNum(3, 4) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "A few needles still scrape you for " << damageDealt << " damage!" << endl;
                } else {
                    int damageDealt = randomNum(6, 8) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Thorn Imp jabs you with a bundle of needles, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Raise Thorns") {
                MAD MAD;
                MAD.defenceValue = 5 + thorns;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                thorns += 2;
                MS.danger += 1;
                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    int damageDealt = randomNum(3, 5) + thorns;
                    playerIP.health -= damageDealt;
                    cout << "Your swing catches the raised thorns! They rip across your hands for " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "MAGIC" || PAD.type == "CRITICAL HIT") {
                    thorns -= 2;
                    makeZeroIfNegative(thorns);
                    cout << "Your power burns away some of the bramble armor before it fully grows!" << endl;
                } else {
                    cout << "The Thorn Imp covers itself in brambles. Its next openings look painful to take." << endl;
                }
            } else if (nameOfAction == "Vine Snare") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    cout << "You move before the vines can tighten around your legs!" << endl;
                } else {
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    PS.temporaryDamageModifier -= 1;
                    cout << "A hooked vine snaps around your ankle! You lose 2 Momentum and your attacks weaken." << endl;
                }
            } else if (nameOfAction == "Cackle") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                MS.temporaryDamageModifier += 1;
                if (PAD.type == "CHARGE") {
                    MS.temporaryDamageModifier += 1;
                    cout << "The Thorn Imp loves that you gave it time. Its thorns sharpen twice over!" << endl;
                } else {
                    cout << "The Thorn Imp cackles until its thorns twitch sharper. Its attack rises!" << endl;
                }
            }
        }
    }
};

class MireLeech : public Encounter {
private:
    bool latched = false;
public:
    explicit MireLeech(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(28, "Mire Leech", {"Latch", "Drain", "Mud Spit", "Submerge"}, {30, 30, 25, 15}
        , {"The Mire Leech coils like a wet rope, preparing to leap...",
           "The Mire Leech pulses sickly red, its mouth still searching for warmth...",
           "The Mire Leech gulps up a mouthful of black mud...",
           "The Mire Leech melts down into the mud beneath it..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (latched) {
            int drainDamage = randomNum(2, 4) + MS.temporaryDamageModifier;
            makeZeroIfNegative(drainDamage);
            playerIP.health -= drainDamage;
            health += drainDamage;
            cout << "The Mire Leech is still latched on and drains " << drainDamage << " health before you can act!" << endl;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Latch") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    latched = false;
                    cout << "Your burst of force blasts the Mire Leech away before it can latch!" << endl;
                } else if (PAD.defenceValue >= 10000) {
                    cout << "The Mire Leech splats into the ground where you were standing." << endl;
                } else {
                    int damageDealt = randomNum(4, 6) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    latched = true;
                    cout << "The Mire Leech clamps onto you, dealing " << damageDealt << " damage and staying attached!" << endl;
                }
            } else if (nameOfAction == "Drain") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    latched = false;
                    cout << "You tear the Mire Leech free before it can feed properly!" << endl;
                } else if (latched) {
                    int damageDealt = randomNum(8, 11) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    health += damageDealt;
                    cout << "The Mire Leech drinks deep, dealing " << damageDealt << " damage and healing itself for the same amount!" << endl;
                } else {
                    cout << "The Mire Leech tries to drain you, but it has not latched on yet." << endl;
                }
            } else if (nameOfAction == "Mud Spit") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit knocks the mud glob out of the air!" << endl;
                } else {
                    int damageDealt = randomNum(3, 5) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 1;
                    cout << "The Mire Leech spits mud into your eyes, dealing " << damageDealt << " damage and weakening your next swings!" << endl;
                }
            } else if (nameOfAction == "Submerge") {
                MAD MAD;
                MAD.defenceValue = 8;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    cout << "Your magic churns the mud and forces the Mire Leech back up!" << endl;
                    latched = false;
                } else {
                    health += randomNum(3, 5);
                    cout << "The Mire Leech hides under the muck and resurfaces looking freshly swollen." << endl;
                }
            }
        }
    }
};

class GlassSpider : public Encounter {
private:
    int webbing = 0;
public:
    explicit GlassSpider(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(32, "Glass Spider", {"Crystal Bite", "Web Line", "Mirror Shell", "Shatter Leap"}, {35, 25, 25, 15}
        , {"The Glass Spider raises its clear fangs and skitters toward your face...",
           "The Glass Spider spins a shining thread between its legs...",
           "The Glass Spider folds into a mirrored shape, reflecting your stance back at you...",
           "The Glass Spider trembles so violently that cracks spread across its body..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Crystal Bite") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit cracks the Spider's fangs before it bites!" << endl;
                    MS.temporaryDamageModifier -= 1;
                } else {
                    int damageDealt = randomNum(7, 10) + webbing + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Glass Spider bites you with needle-clear fangs, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Web Line") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    cout << "You duck under the glassy web before it catches you." << endl;
                } else {
                    webbing += 1;
                    PS.momentum -= 1;
                    makeZeroIfNegative(PS.momentum);
                    cout << "A glass web wraps around you. Future bites hurt more, and you lose 1 Momentum." << endl;
                }
            } else if (nameOfAction == "Mirror Shell") {
                MAD MAD;
                MAD.defenceValue = 10;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    int damageDealt = randomNum(4, 7);
                    playerIP.health -= damageDealt;
                    cout << "Your strike glances off the mirror shell and sprays glass into you for " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "MAGIC") {
                    cout << "The mirror shell fogs over under your magic." << endl;
                    webbing -= 1;
                    makeZeroIfNegative(webbing);
                } else {
                    cout << "The Spider waits inside its mirror shell, nearly impossible to read." << endl;
                }
            } else if (nameOfAction == "Shatter Leap") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                health -= 3;
                if (PAD.type == "CRITICAL HIT" || PAD.type == "PARRY") {
                    cout << "You catch the leap perfectly, and the Spider shatters itself against your counter!" << endl;
                    health -= randomNum(4, 6);
                } else {
                    int damageDealt = randomNum(12, 16) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Glass Spider explodes through the air, cutting you for " << damageDealt << " damage while hurting itself!" << endl;
                }
            }
        }
    }
};

class IronScarab : public Encounter {
private:
    int armor = 6;
public:
    explicit IronScarab(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(40, "Iron Scarab", {"Mandible Crush", "Shell Guard", "Rolling Charge", "Polish Armor"}, {35, 25, 25, 15}
        , {"The Iron Scarab opens its metal mandibles and scrapes them together...",
           "The Iron Scarab tucks its legs beneath its heavy shell...",
           "The Iron Scarab rocks backward, ready to roll like a cannonball...",
           "The Iron Scarab rubs its shell until sparks dance over the iron..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Mandible Crush") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit dents one mandible shut before it can crush you!" << endl;
                    armor -= 1;
                    makeZeroIfNegative(armor);
                } else if (PAD.type == "PARRY") {
                    cout << "You parry between the mandibles, forcing them apart!" << endl;
                    armor -= 1;
                    makeZeroIfNegative(armor);
                } else {
                    int damageDealt = randomNum(8, 12) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Iron Scarab crushes down with its mandibles, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Shell Guard") {
                MAD MAD;
                MAD.defenceValue = armor + 8;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    armor -= 2;
                    makeZeroIfNegative(armor);
                    cout << "Magic seeps through cracks in the shell, weakening the armor!" << endl;
                } else if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    int damageDealt = randomNum(2, 4) + armor;
                    playerIP.health -= damageDealt;
                    cout << "Your weapon jars against iron plating. The shock hurts you for " << damageDealt << " damage!" << endl;
                } else {
                    cout << "The Scarab's iron shell absorbs the moment." << endl;
                }
            } else if (nameOfAction == "Rolling Charge") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "PARRY" || PAD.defenceValue >= 10000) {
                    cout << "You use the Scarab's momentum against it and send it rolling past you!" << endl;
                    MS.danger = 0;
                } else if (PAD.type == "CHARGE") {
                    int damageDealt = randomNum(18, 22) + armor + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "You pause for too long. The Scarab slams into you at full speed for " << damageDealt << " damage!" << endl;
                } else {
                    int damageDealt = randomNum(11, 15) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Iron Scarab rolls over your footing, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Polish Armor") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                armor += 2;
                if (PAD.type == "CRITICAL HIT") {
                    armor -= 3;
                    makeZeroIfNegative(armor);
                    cout << "Your Critical Hit gouges through the newly polished armor!" << endl;
                } else {
                    cout << "The Iron Scarab polishes its shell. Its armor rises to " << armor << "." << endl;
                }
            }
        }
    }
};

class EchoBat : public Encounter {
private:
    int echo = 0;
public:
    explicit EchoBat(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(22, "Echo Bat", {"Dive", "Screech", "Wing Buffet", "Echo Copy"}, {35, 25, 25, 15}
        , {"The Echo Bat vanishes upward, then drops toward your head...",
           "The Echo Bat inhales until its tiny ribs shake...",
           "The Echo Bat beats its wings hard enough to bend the grass...",
           "The Echo Bat listens closely, waiting to mimic your rhythm..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Dive") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit catches the Echo Bat mid-dive!" << endl;
                } else {
                    int damageDealt = randomNum(5, 8) + echo + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Echo Bat dives into you, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Screech") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    echo -= 1;
                    makeZeroIfNegative(echo);
                    cout << "Your magic disrupts the sound wave and calms the echoes." << endl;
                } else {
                    int damageDealt = randomNum(3, 5) + echo + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    echo += 1;
                    cout << "The screech rattles your skull for " << damageDealt << " damage. You lose 2 Momentum, and the cave echoes grow louder." << endl;
                }
            } else if (nameOfAction == "Wing Buffet") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "PARRY") {
                    cout << "You time the wingbeat and smack the Echo Bat out of its rhythm!" << endl;
                    echo -= 1;
                    makeZeroIfNegative(echo);
                } else if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit interrupts the wing buffet completely!" << endl;
                } else {
                    int damageDealt = randomNum(4, 7) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Echo Bat's wings buffet you backward for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Echo Copy") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CHARGE") {
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    echo += 2;
                    cout << "The Echo Bat copies your careful breathing and steals the rhythm. You lose 2 Momentum." << endl;
                } else if (PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT") {
                    echo += 1;
                    cout << "Every weapon swing comes back as sound. The Echo Bat's next attacks grow louder." << endl;
                } else {
                    cout << "The Echo Bat fails to find a rhythm worth copying." << endl;
                }
            }
        }
    }
};

class AshenHound : public Encounter {
private:
    bool marked = false;
public:
    explicit AshenHound(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(36, "Ashen Hound", {"Bite", "Ember Mark", "Smoke Dash", "Howl"}, {35, 25, 25, 15}
        , {"The Ashen Hound bares cracked, ember-lit teeth...",
           "The Ashen Hound exhales a glowing brand toward your chest...",
           "The Ashen Hound dissolves into smoke and circles behind you...",
           "The Ashen Hound lifts its burnt muzzle and howls through the trees..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Bite") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit knocks the Ashen Hound's jaws away!" << endl;
                } else if (PAD.type == "PARRY") {
                    cout << "You parry the bite and scrape ash from its jaw." << endl;
                    marked = false;
                } else {
                    int bonus = 0;
                    if (marked) bonus = 5;
                    int damageDealt = randomNum(8, 11) + bonus + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Ashen Hound bites down, dealing " << damageDealt << " damage!" << endl;
                    if (marked) {
                        marked = false;
                        cout << "The ember mark bursts as the bite lands." << endl;
                    }
                }
            } else if (nameOfAction == "Ember Mark") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.defenceValue >= 10000 || PAD.type == "MAGIC") {
                    marked = false;
                    cout << "You avoid or smother the ember mark before it can brand you." << endl;
                } else {
                    marked = true;
                    MS.danger += 1;
                    cout << "A burning mark settles over your heart. The next bite will hurt much more." << endl;
                }
            } else if (nameOfAction == "Smoke Dash") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit catches the Hound before it fully becomes smoke!" << endl;
                } else {
                    int damageDealt = randomNum(5, 7) + MS.danger + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Ashen Hound dashes through you as smoke, burning you for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Howl") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                MS.danger += 2;
                if (PAD.type == "CHARGE") {
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    cout << "The howl ruins your concentration. You lose 2 Momentum while the Hound grows more dangerous." << endl;
                } else {
                    cout << "Ash drifts upward as the Hound's danger rises." << endl;
                }
            }
        }
    }
};

class GraveMoss : public Encounter {
private:
    int growth = 0;
public:
    explicit GraveMoss(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(34, "Grave Moss", {"Creeping Grasp", "Bloom Spores", "Feed on Stone", "Root Shield"}, {30, 25, 20, 25}
        , {"The Grave Moss creeps toward your boots like a slow green tide...",
           "Pale bulbs swell across the Grave Moss and begin to shake...",
           "The Grave Moss pushes roots into a cracked grave marker...",
           "The Grave Moss curls itself into a thick knot of roots..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Creeping Grasp") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "PARRY" || PAD.defenceValue >= 10000) {
                    cout << "You avoid the slow grasping roots!" << endl;
                } else {
                    int damageDealt = randomNum(6, 9) + growth + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.momentum -= 1;
                    makeZeroIfNegative(PS.momentum);
                    cout << "Roots climb your legs and squeeze for " << damageDealt << " damage. You lose 1 Momentum." << endl;
                }
            } else if (nameOfAction == "Bloom Spores") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    growth -= 1;
                    makeZeroIfNegative(growth);
                    cout << "Your magic burns the spores before they can spread." << endl;
                } else {
                    PS.temporaryDamageModifier -= 1;
                    growth += 1;
                    cout << "Spores fill your lungs. Your attacks weaken while the Grave Moss spreads." << endl;
                }
            } else if (nameOfAction == "Feed on Stone") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int healAmount = randomNum(5, 8) + growth;
                health += healAmount;
                growth += 1;
                if (PAD.type == "CRITICAL HIT") {
                    health -= healAmount;
                    growth -= 1;
                    makeZeroIfNegative(growth);
                    cout << "Your Critical Hit cuts through the feeding roots and stops the healing!" << endl;
                } else {
                    cout << "The Grave Moss feeds on old stone and heals for " << healAmount << "." << endl;
                }
            } else if (nameOfAction == "Root Shield") {
                MAD MAD;
                MAD.defenceValue = 6 + growth;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    growth += 1;
                    cout << "Your weapon catches in the roots, giving the Grave Moss more material to grow around." << endl;
                } else if (PAD.type == "MAGIC") {
                    growth -= 2;
                    makeZeroIfNegative(growth);
                    cout << "Magic blackens the root shield and slows the moss growth." << endl;
                } else {
                    cout << "The Grave Moss waits behind its roots." << endl;
                }
            }
        }
    }
};

class HollowSquire : public Encounter {
private:
    bool guarding = false;
public:
    explicit HollowSquire(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(38, "Hollow Squire", {"Rusty Slash", "Shield Up", "Desperate Thrust", "Old Training"}, {35, 25, 25, 15}
        , {"The Hollow Squire drags a rusty sword into a familiar stance...",
           "The Hollow Squire raises a dented shield and hides behind it...",
           "The Hollow Squire lowers its shoulder for a desperate thrust...",
           "The Hollow Squire repeats ancient footwork from a forgotten lesson..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Rusty Slash") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                if (guarding) MAD.defenceValue = 4;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                guarding = false;
                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit knocks the rusty slash aside!" << endl;
                } else if (PAD.type == "PARRY") {
                    cout << "You parry the old sword cleanly and break the Squire's confidence." << endl;
                    MS.temporaryDamageModifier -= 1;
                } else {
                    int damageDealt = randomNum(8, 12) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Hollow Squire's rusty slash deals " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Shield Up") {
                MAD MAD;
                MAD.defenceValue = 12;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                guarding = true;
                if (PAD.type == "MAGIC") {
                    guarding = false;
                    cout << "Your magic curls around the shield and ruins the Squire's guard." << endl;
                } else if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    PS.momentum -= 1;
                    makeZeroIfNegative(PS.momentum);
                    cout << "Your weapon bangs against the shield. The impact costs you 1 Momentum." << endl;
                } else {
                    cout << "The Hollow Squire sets its shield and prepares to punish a careless swing." << endl;
                }
            } else if (nameOfAction == "Desperate Thrust") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                guarding = false;
                if (PAD.type == "PARRY") {
                    cout << "You parry the desperate thrust and leave the Hollow Squire wide open!" << endl;
                    health -= randomNum(4, 7);
                } else if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit crushes the desperate thrust before it begins!" << endl;
                } else {
                    int bonus = 0;
                    if (health < 15) bonus = 5;
                    int damageDealt = randomNum(10, 14) + bonus + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Hollow Squire commits everything to one thrust, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Old Training") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                MS.temporaryDamageModifier += 1;
                if (PAD.type == "CHARGE") {
                    PS.momentum += 1;
                    cout << "You study the same old training pattern and gain 1 extra Momentum, but the Squire's attack rises." << endl;
                } else {
                    cout << "The Hollow Squire remembers a fragment of training. Its attack rises." << endl;
                }
            }
        }
    }
};

class LanternBearer : public Encounter {
private:
    int lanternCharge = 0;
public:
    explicit LanternBearer(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(33, "Lantern Bearer", {"Hook Swipe", "Raise Lantern", "Blinding Flash", "Oil Splash"}, {35, 25, 20, 20}
        , {"The Lantern Bearer swings a hooked lantern chain toward you...",
           "The Lantern Bearer lifts its lantern high, feeding it with a whispered prayer...",
           "The lantern glass glows white-hot and points straight at your eyes...",
           "The Lantern Bearer sloshes burning oil around the arena..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Hook Swipe") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit snaps the lantern chain away from you!" << endl;
                } else if (PAD.type == "PARRY") {
                    cout << "You parry the hook and make the lantern sputter." << endl;
                    lanternCharge -= 1;
                    makeZeroIfNegative(lanternCharge);
                } else {
                    int damageDealt = randomNum(7, 10) + lanternCharge + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The hooked chain tears across you for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Raise Lantern") {
                MAD MAD;
                MAD.defenceValue = 4;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                lanternCharge += 2;
                MS.danger += 1;
                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    lanternCharge -= 2;
                    makeZeroIfNegative(lanternCharge);
                    cout << "You disrupt the lantern before it can gather much light." << endl;
                } else {
                    cout << "The lantern drinks in the darkness. Its charge rises to " << lanternCharge << "." << endl;
                }
            } else if (nameOfAction == "Blinding Flash") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.defenceValue >= 10000) {
                    cout << "You roll away from the flash before it catches your eyes." << endl;
                } else {
                    int damageDealt = randomNum(4, 6) + lanternCharge + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 2;
                    cout << "The lantern flashes white and blinds you, dealing " << damageDealt << " damage and lowering your attack!" << endl;
                    lanternCharge = 0;
                }
            } else if (nameOfAction == "Oil Splash") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    cout << "Your magic scatters the oil before it can catch flame." << endl;
                } else {
                    int damageDealt = randomNum(5, 8) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    MS.danger += 1;
                    cout << "Burning oil splashes at your feet for " << damageDealt << " damage. The fight grows more dangerous." << endl;
                }
            }
        }
    }
};

class AncientEnt : public Encounter {
private:
    int roots = 0;
    int bark = 0;
public:
    explicit AncientEnt(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(85, "Ancient Ent", {"Branch Hammer", "Root Prison", "Heartwood Guard", "Sap Renewal", "Spore Bloom", "Forest's Judgment"}, {25, 20, 18, 14, 13, 10}
        , {"The Ancient Ent raises one arm like a falling tree...",
           "The ground cracks as old roots begin searching for your ankles...",
           "The Ent pulls its bark inward, becoming still as a wall of living wood...",
           "Golden sap leaks from the Ent's wounds and crawls back into place...",
           "The Ent shakes its crown, and pale spores drift through the air...",
           "Every branch points at you. The forest seems to hold its breath..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (roots > 0) {
            PS.momentum -= 1;
            makeZeroIfNegative(PS.momentum);
            cout << "Old roots tug at your legs before you can move. You lose 1 Momentum." << endl;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Branch Hammer") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                MAD.defenceValue = bark;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit splits the falling branch and cancels the blow!" << endl;
                    bark -= 2;
                    makeZeroIfNegative(bark);
                } else if (PAD.type == "PARRY") {
                    int damageDealt = randomNum(8, 11) + roots + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    bark -= 1;
                    makeZeroIfNegative(bark);
                    cout << "You turn the branch aside, but its weight still crushes your guard for " << damageDealt << " damage!" << endl;
                } else {
                    int damageDealt = randomNum(17, 23) + roots + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Ent's branch comes down like a treefall, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Root Prison") {
                MAD MAD;
                MAD.defenceValue = bark / 2;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    roots -= 1;
                    makeZeroIfNegative(roots);
                    cout << "You roll clear before the roots close around you." << endl;
                } else if (PAD.type == "MAGIC" || PAD.type == "CRITICAL HIT") {
                    roots -= 2;
                    makeZeroIfNegative(roots);
                    cout << "Your power tears apart the searching roots before they can bind you." << endl;
                } else {
                    roots += 2;
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    cout << "Roots lock around your legs. The Ent gains 2 Root Grip, and you lose 2 Momentum!" << endl;
                }
            } else if (nameOfAction == "Heartwood Guard") {
                MAD MAD;
                MAD.defenceValue = 10 + bark;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                bark += 3;
                MS.danger += 1;
                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    int damageDealt = randomNum(5, 8) + bark;
                    playerIP.health -= damageDealt;
                    cout << "Your weapon bites into armored heartwood and splinters back at you for " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "MAGIC") {
                    bark -= 2;
                    makeZeroIfNegative(bark);
                    cout << "Magic burns between the layers of bark, preventing some of the armor from forming." << endl;
                } else {
                    cout << "The Ent becomes harder to injure. Its bark thickens around its ancient heart." << endl;
                }
            } else if (nameOfAction == "Sap Renewal") {
                MAD MAD;
                MAD.defenceValue = bark;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    cout << "You rupture the glowing sap before it can seal the Ent's wounds." << endl;
                    MS.temporaryDamageModifier -= 1;
                } else {
                    int healAmount = randomNum(9, 14) + bark;
                    health += healAmount;
                    roots += 1;
                    cout << "Golden sap seals the Ent's wounds. It heals " << healAmount << " health and spreads more roots." << endl;
                }
            } else if (nameOfAction == "Spore Bloom") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    cout << "You escape the thickest cloud of spores." << endl;
                } else {
                    int damageDealt = randomNum(5, 8) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 2;
                    if (PAD.type == "CHARGE") {
                        PS.momentum -= 2;
                        makeZeroIfNegative(PS.momentum);
                        cout << "You inhale deeply while thinking. The spores punish that pause brutally." << endl;
                    }
                    cout << "The spores choke your lungs, dealing " << damageDealt << " damage and weakening your attacks." << endl;
                }
            } else if (nameOfAction == "Forest's Judgment") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int damageDealt = randomNum(12, 18) + roots + bark + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    damageDealt /= 2;
                    cout << "You disrupt the forest's focus, cutting the judgment in half." << endl;
                }
                playerIP.health -= damageDealt;
                roots -= 2;
                bark -= 1;
                makeZeroIfNegative(roots);
                makeZeroIfNegative(bark);
                cout << "Branches, stones, and roots strike all at once for " << damageDealt << " damage! The Ent spends some of its built-up power." << endl;
            }
        }
    }
};

class ReforgedKnight : public Encounter {
private:
    int heat = 0;
    int armor = 4;
public:
    explicit ReforgedKnight(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(76, "Reforged Knight", {"Furnace Cleave", "Magnetize Armor", "Ember Riposte", "Scrap Barrage", "Temper Blade", "Final Bellows"}, {26, 18, 18, 16, 14, 8}
        , {"The Reforged Knight drags a glowing sword across the ground...",
           "Loose nails, arrowheads, and broken blades rattle toward the Knight's armor...",
           "The Knight lowers its blade and waits for your first mistake...",
           "The Knight's back opens like a furnace full of shrapnel...",
           "The Knight holds its sword in its own chest-flame, heating the edge white...",
           "The Knight's furnace-heart starts roaring far too loudly..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (heat > 0) {
            MS.danger += 1;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Furnace Cleave") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                MAD.defenceValue = armor;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    heat -= 2;
                    makeZeroIfNegative(heat);
                    cout << "Your Critical Hit dents the furnace plate and vents some of the Knight's heat!" << endl;
                } else {
                    int damageDealt = randomNum(13, 18) + heat + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Reforged Knight cleaves with a furnace-hot blade for " << damageDealt << " damage!" << endl;
                    if (PAD.type == "PARRY") {
                        heat += 1;
                        cout << "Your parry showers sparks across both of you. The Knight gets hotter." << endl;
                    }
                }
            } else if (nameOfAction == "Magnetize Armor") {
                MAD MAD;
                MAD.defenceValue = 12 + armor;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                armor += 3;
                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    PS.temporaryDamageModifier -= 2;
                    cout << "Your weapon pulls toward the Knight's armor. Your balance and attack power suffer." << endl;
                } else if (PAD.type == "MAGIC") {
                    armor -= 3;
                    makeZeroIfNegative(armor);
                    cout << "Magic scrambles the magnetic pull before the armor can thicken." << endl;
                } else {
                    cout << "Metal scraps lock onto the Knight, raising its armor." << endl;
                }
            } else if (nameOfAction == "Ember Riposte") {
                MAD MAD;
                MAD.defenceValue = 8 + armor;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT") {
                    int damageDealt = randomNum(11, 15) + heat + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Knight catches your strike and answers with a burning counter for " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "CHARGE") {
                    heat += 2;
                    cout << "The Knight waits with you, feeding its furnace while you think." << endl;
                } else {
                    cout << "You refuse the Knight's trap, and its counterstance finds no opening." << endl;
                }
            } else if (nameOfAction == "Scrap Barrage") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your strike knocks the furnace hatch closed before the barrage fires." << endl;
                } else {
                    int damageDealt = randomNum(8, 12) + armor / 2 + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.momentum -= 1;
                    makeZeroIfNegative(PS.momentum);
                    cout << "A storm of hot scrap peppers you for " << damageDealt << " damage and knocks away 1 Momentum." << endl;
                }
            } else if (nameOfAction == "Temper Blade") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                heat += 3;
                MS.temporaryDamageModifier += 1;
                if (PAD.type == "MAGIC") {
                    heat -= 2;
                    makeZeroIfNegative(heat);
                    cout << "Your magic cools the blade before it can fully temper." << endl;
                } else {
                    cout << "The Knight tempers its blade in its own chest-fire. Its future attacks become nastier." << endl;
                }
            } else if (nameOfAction == "Final Bellows") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int damageDealt = randomNum(9, 13) + heat * 2 + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "DODGE") {
                    damageDealt /= 2;
                    cout << "You escape the center of the furnace blast." << endl;
                }
                playerIP.health -= damageDealt;
                heat = 0;
                armor -= 2;
                makeZeroIfNegative(armor);
                cout << "The Knight vents all of its heat in a roaring blast for " << damageDealt << " damage!" << endl;
            }
        }
    }
};

class CandleWraith : public Encounter {
private:
    int haunting = 0;
    bool faded = false;
public:
    explicit CandleWraith(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(64, "Candle Wraith", {"Blue Flame", "Flickerstep", "Soul Wick", "Snuff Breath", "Mourning Glow", "Waxen Grasp"}, {24, 18, 18, 16, 14, 10}
        , {"A blue candle-flame blooms inside the Wraith's hollow ribs...",
           "The Candle Wraith's outline flickers between here and somewhere colder...",
           "The Wraith pinches its own flame and reaches for the souls inside you...",
           "The Wraith inhales, and the light around you begins to die...",
           "The Wraith bows its head in silent grief, growing brighter...",
           "Hot wax drips upward from the floor, trying to harden around your wrists..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (faded) {
            cout << "The Candle Wraith is half-faded, making your next blow harder to land cleanly." << endl;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Blue Flame") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Normal";
                if (faded) MAD.defenceValue = 8;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    faded = false;
                    cout << "Your Critical Hit scatters the blue flame before it reaches you." << endl;
                } else {
                    int damageDealt = randomNum(9, 13) + haunting + MS.temporaryDamageModifier - PAD.defenceValue / 2;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "Cold blue fire passes through your guard for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Flickerstep") {
                MAD MAD;
                MAD.defenceValue = 14;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                faded = true;
                haunting += 1;
                if (PAD.type == "MAGIC") {
                    faded = false;
                    cout << "Magic pins the Wraith's shadow to the floor before it can fully vanish." << endl;
                } else {
                    cout << "The Wraith flickers away from solid harm. Its next attack will be guided by haunting light." << endl;
                }
            } else if (nameOfAction == "Soul Wick") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (playerIP.soul > 0 && PAD.type != "MAGIC") {
                    int stolenSouls = randomNum(1, 2);
                    playerIP.soul -= stolenSouls;
                    makeZeroIfNegative(playerIP.soul);
                    health += stolenSouls * 6;
                    haunting += stolenSouls;
                    cout << "The Wraith burns " << stolenSouls << " of your Souls as candlewick and heals itself." << endl;
                } else if (PAD.type == "MAGIC") {
                    cout << "Your own soulcraft blocks the Wraith's attempt to steal from you." << endl;
                } else {
                    haunting += 1;
                    cout << "Finding no Souls to burn, the Wraith feeds on the room's grief instead." << endl;
                }
            } else if (nameOfAction == "Snuff Breath") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    cout << "You roll out of the dead air before it reaches your lungs." << endl;
                } else {
                    int damageDealt = randomNum(5, 9) + haunting + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.momentum -= 2;
                    PS.temporaryDamageModifier -= 1;
                    makeZeroIfNegative(PS.momentum);
                    cout << "The Wraith snuffs the air from your chest for " << damageDealt << " damage. You lose 2 Momentum." << endl;
                }
            } else if (nameOfAction == "Mourning Glow") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                haunting += 2;
                MS.danger += 1;
                if (PAD.type == "CHARGE") {
                    playerIP.soul -= 1;
                    makeZeroIfNegative(playerIP.soul);
                    cout << "You pause long enough to hear the Wraith's grief. It costs you 1 Soul." << endl;
                }
                cout << "The Candle Wraith glows brighter with old sorrow. Its haunting grows." << endl;
            } else if (nameOfAction == "Waxen Grasp") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT" || PAD.defenceValue >= 10000) {
                    cout << "You break away before the wax can harden." << endl;
                } else {
                    int damageDealt = randomNum(6, 10) + haunting + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 2;
                    cout << "Spirit-wax clamps around your weapon arm for " << damageDealt << " damage and weakens your attacks." << endl;
                }
            }
        }
    }
};

class CathedralGargoyle : public Encounter {
private:
    int altitude = 0;
    int stoneSkin = 6;
public:
    explicit CathedralGargoyle(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(82, "Cathedral Gargoyle", {"Stone Talons", "Roof Dive", "Petrifying Glare", "Granite Shell", "Bell Roar", "Wingbeat"}, {24, 16, 18, 16, 14, 12}
        , {"The Gargoyle crawls forward on all fours, talons carving sparks...",
           "The Gargoyle climbs upward, lining itself with the broken ceiling...",
           "The Gargoyle's stone eyes grind open and fix on your face...",
           "The Gargoyle folds its wings around itself like a chapel wall...",
           "The Gargoyle opens its jaw, and a cracked bronze bell rings inside its throat...",
           "The Gargoyle beats its wings, filling the room with dust and falling stone..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (altitude > 0) {
            cout << "The Gargoyle is above you, making direct attacks less reliable." << endl;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Stone Talons") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                MAD.defenceValue = stoneSkin;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    stoneSkin -= 2;
                    makeZeroIfNegative(stoneSkin);
                    cout << "Your Critical Hit chips away some of the Gargoyle's stone hide!" << endl;
                } else {
                    int damageDealt = randomNum(11, 15) + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The Gargoyle rakes you with stone talons for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Roof Dive") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                MAD.defenceValue = altitude > 0 ? 10 : 4;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.defenceValue >= 10000) {
                    cout << "You roll away as the Gargoyle pulverizes the ground where you stood." << endl;
                    altitude = 0;
                } else if (PAD.type == "PARRY") {
                    int damageDealt = randomNum(7, 10) + altitude + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    altitude = 0;
                    cout << "You deflect the dive, but its mass still slams through your stance for " << damageDealt << " damage!" << endl;
                } else {
                    int damageDealt = randomNum(18, 25) + altitude * 3 + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    altitude = 0;
                    cout << "The Gargoyle drops from above like a cathedral bell, dealing " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Petrifying Glare") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    cout << "Your magic breaks eye contact before the petrification takes root." << endl;
                } else {
                    PS.momentum -= 2;
                    PS.temporaryDamageModifier -= 2;
                    makeZeroIfNegative(PS.momentum);
                    cout << "Your muscles stiffen under the Gargoyle's stare. You lose 2 Momentum and attack power." << endl;
                }
            } else if (nameOfAction == "Granite Shell") {
                MAD MAD;
                MAD.defenceValue = 16 + stoneSkin;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                stoneSkin += 3;
                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    int damageDealt = randomNum(6, 9) + stoneSkin;
                    playerIP.health -= damageDealt;
                    cout << "Your strike rebounds from the shell, sending stone chips into you for " << damageDealt << " damage!" << endl;
                } else if (PAD.type == "MAGIC") {
                    stoneSkin -= 2;
                    makeZeroIfNegative(stoneSkin);
                    cout << "Magic gets into the cracks and prevents the shell from fully sealing." << endl;
                } else {
                    cout << "The Gargoyle's body hardens into deeper granite." << endl;
                }
            } else if (nameOfAction == "Bell Roar") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int damageDealt = randomNum(7, 11) + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                MS.danger += 2;
                if (PAD.type == "CHARGE") {
                    PS.momentum -= 3;
                    makeZeroIfNegative(PS.momentum);
                    cout << "The bell-roar shatters your concentration and steals the Momentum you were gathering." << endl;
                }
                cout << "The bronze roar rattles your bones for " << damageDealt << " damage. Danger rises." << endl;
            } else if (nameOfAction == "Wingbeat") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                altitude += 1;
                int damageDealt = randomNum(4, 7) + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                if (PAD.type != "DODGE") {
                    PS.temporaryDamageModifier -= 1;
                }
                cout << "Dust and stone fragments batter you for " << damageDealt << " damage as the Gargoyle takes height." << endl;
            }
        }
    }
};

class SiltHydra : public Encounter {
private:
    int heads = 3;
    int acid = 0;
public:
    explicit SiltHydra(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(90, "Silt Hydra", {"Many-Mouth Bite", "Acid Spray", "Regrow Head", "Dragging Current", "Tail Flood", "Devour Opening"}, {25, 18, 17, 15, 15, 10}
        , {"The Silt Hydra's heads argue over which part of you to bite first...",
           "The Hydra's throats bulge with yellow river-acid...",
           "A severed stump twitches, trying to bloom into another head...",
           "Mud swirls around your boots as the Hydra pulls the ground like water...",
           "The Hydra coils its tail through the silt, preparing to flood the chamber...",
           "All of the Hydra's heads suddenly go silent, watching for an opening..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Many-Mouth Bite") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    heads -= 1;
                    if (heads < 1) heads = 1;
                    cout << "Your Critical Hit takes one snapping head out of the fight!" << endl;
                } else {
                    int damageDealt = randomNum(3, 5) * heads + acid + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << heads << " Hydra heads bite in a frenzy for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Acid Spray") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    cout << "You avoid most of the river-acid." << endl;
                    acid += 1;
                } else {
                    int damageDealt = randomNum(7, 10) + acid + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 2;
                    acid += 1;
                    cout << "Acid burns across your gear for " << damageDealt << " damage and weakens your next attacks." << endl;
                }
            } else if (nameOfAction == "Regrow Head") {
                MAD MAD;
                MAD.defenceValue = 7;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    cout << "You cauterize the stump before another head can grow." << endl;
                    acid -= 1;
                    makeZeroIfNegative(acid);
                } else {
                    heads += 1;
                    health += randomNum(5, 8);
                    cout << "A new head tears free from the stump. The Hydra now has " << heads << " heads!" << endl;
                }
            } else if (nameOfAction == "Dragging Current") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE") {
                    cout << "You leap out of the sucking mud before it drags you in." << endl;
                } else {
                    PS.momentum -= 3;
                    makeZeroIfNegative(PS.momentum);
                    int damageDealt = randomNum(4, 7) + heads - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The muddy current drags you off balance for " << damageDealt << " damage and strips 3 Momentum." << endl;
                }
            } else if (nameOfAction == "Tail Flood") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int damageDealt = randomNum(10, 15) + acid + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "PARRY") {
                    damageDealt /= 2;
                    cout << "You partially deflect the tail before the flood hits." << endl;
                }
                playerIP.health -= damageDealt;
                if (PAD.type == "CHARGE") {
                    heads += 1;
                    cout << "The Hydra uses your hesitation to let another head surface from the silt." << endl;
                }
                cout << "The Hydra sweeps a wave of silt into you for " << damageDealt << " damage!" << endl;
            } else if (nameOfAction == "Devour Opening") {
                MAD MAD;
                MAD.defenceValue = heads;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT" || PAD.type == "CHARGE") {
                    int damageDealt = randomNum(9, 13) + heads * 2 + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    health += damageDealt / 2;
                    cout << "The Hydra finds the opening it wanted and devours your guard for " << damageDealt << " damage, healing from the bite!" << endl;
                } else {
                    cout << "You deny the Hydra a clean opening." << endl;
                }
            }
        }
    }
};

class MarionetteCoven : public Encounter {
private:
    int strings = 0;
    int dolls = 2;
public:
    explicit MarionetteCoven(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(72, "Marionette Coven", {"Needle Dance", "String Yank", "Puppet Shield", "Forced Bow", "Cut Your Strings", "Doll Swarm"}, {24, 18, 18, 14, 14, 12}
        , {"Several wooden dolls raise silver needles in perfect unison...",
           "Invisible strings snap tight around your wrists...",
           "One doll leaps in front of the others with a painted smile...",
           "The Coven bows, and the strings try to make you bow back...",
           "The dolls lift tiny scissors toward the air above your soul...",
           "Cupboards burst open as more dolls crawl into the fight..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (strings > 0) {
            cout << "The Coven's strings are still on you, tugging at every decision." << endl;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Needle Dance") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    dolls -= 1;
                    if (dolls < 1) dolls = 1;
                    cout << "Your Critical Hit smashes one of the dancing dolls!" << endl;
                } else {
                    int damageDealt = randomNum(3, 5) * dolls + strings + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << dolls << " dolls needle-dance across you for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "String Yank") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "DODGE" || PAD.type == "MAGIC") {
                    strings -= 1;
                    makeZeroIfNegative(strings);
                    cout << "You slip the strings before they can tighten." << endl;
                } else {
                    strings += 2;
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    cout << "Strings yank your joints out of rhythm. The Coven gains control and you lose 2 Momentum." << endl;
                }
            } else if (nameOfAction == "Puppet Shield") {
                MAD MAD;
                MAD.defenceValue = 10 + dolls * 3;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    cout << "A doll takes the hit and laughs with someone else's voice." << endl;
                    dolls -= 1;
                    if (dolls < 1) dolls = 1;
                    strings += 1;
                } else if (PAD.type == "MAGIC") {
                    dolls -= 1;
                    strings -= 1;
                    if (dolls < 1) dolls = 1;
                    makeZeroIfNegative(strings);
                    cout << "Magic burns through the decoy and several hidden strings." << endl;
                } else {
                    cout << "The Coven hides behind a wall of puppets." << endl;
                }
            } else if (nameOfAction == "Forced Bow") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (strings > 0 && PAD.type != "MAGIC") {
                    int damageDealt = randomNum(5, 8) + strings - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 2;
                    cout << "The strings force you into a painful bow for " << damageDealt << " damage and ruin your next attack angle." << endl;
                } else {
                    cout << "The Coven bows alone. Without enough strings, the gesture has little power." << endl;
                }
            } else if (nameOfAction == "Cut Your Strings") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (playerIP.soul > 0 && PAD.type != "MAGIC") {
                    playerIP.soul -= 2;
                    makeZeroIfNegative(playerIP.soul);
                    strings += 1;
                    cout << "Tiny scissors cut at the strings between you and your Souls. You lose up to 2 Souls." << endl;
                } else if (PAD.type == "MAGIC") {
                    strings -= 2;
                    makeZeroIfNegative(strings);
                    cout << "Your magic reveals the real strings and severs them." << endl;
                } else {
                    cout << "The scissors click hungrily, but find no Souls to cut loose." << endl;
                }
            } else if (nameOfAction == "Doll Swarm") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                dolls += 1;
                int damageDealt = randomNum(4, 6) + dolls + strings + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "CRITICAL HIT") {
                    damageDealt /= 2;
                    dolls -= 1;
                    if (dolls < 1) dolls = 1;
                    cout << "You scatter part of the swarm before it reaches you." << endl;
                }
                playerIP.health -= damageDealt;
                cout << "New dolls crawl over you in a clicking swarm for " << damageDealt << " damage!" << endl;
            }
        }
    }
};

class StarlessOracle : public Encounter {
private:
    int omens = 0;
    bool mirrorFate = false;
public:
    explicit StarlessOracle(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(70, "Starless Oracle", {"Foretell Wound", "Doom Pulse", "Memory Tax", "Mirror Fate", "Blind Star", "Unask"}, {20, 20, 17, 17, 14, 12}
        , {"The Starless Oracle whispers the exact sound you will make when hurt...",
           "A black pulse gathers where the Oracle's eyes should be...",
           "The Oracle opens a book full of memories you do not remember writing...",
           "The Oracle raises a mirror that reflects you dying in several possible ways...",
           "A starless light appears above you, bright without shining...",
           "The Oracle asks a question, then removes the question from the world..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (mirrorFate) {
            cout << "Mirror Fate still hangs in the air. Aggression may hurt you back." << endl;
        }

        if (getHealth() > 0) {
            if (nameOfAction == "Foretell Wound") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                omens += 2;
                MS.danger += 1;
                if (PAD.type == "CHARGE") {
                    omens += 1;
                    cout << "The Oracle uses your quiet thought to read even further ahead." << endl;
                } else if (PAD.type == "MAGIC") {
                    omens -= 1;
                    makeZeroIfNegative(omens);
                    cout << "Your magic makes one part of the prophecy contradict itself." << endl;
                }
                cout << "The Oracle stores a prophecy. It now has " << omens << " Omens." << endl;
            } else if (nameOfAction == "Doom Pulse") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int damageDealt = randomNum(8, 12) + omens + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "DODGE") {
                    damageDealt /= 2;
                    cout << "You avoid the center of the black pulse." << endl;
                }
                playerIP.health -= damageDealt;
                cout << "Doom rolls through your body for " << damageDealt << " damage." << endl;
            } else if (nameOfAction == "Memory Tax") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    cout << "You protect your memories with soulcraft before the Oracle can tax them." << endl;
                } else {
                    PS.momentum -= 2;
                    makeZeroIfNegative(PS.momentum);
                    if (playerIP.soul > 0) {
                        playerIP.soul -= 1;
                        makeZeroIfNegative(playerIP.soul);
                        omens += 1;
                        cout << "The Oracle takes one Soul and two Momentum as payment for a memory you forgot." << endl;
                    } else {
                        PS.temporaryDamageModifier -= 2;
                        cout << "With no Soul to take, the Oracle taxes your confidence instead." << endl;
                    }
                }
            } else if (nameOfAction == "Mirror Fate") {
                MAD MAD;
                MAD.defenceValue = 9 + omens;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                mirrorFate = true;
                if (PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    int damageDealt = randomNum(5, 8) + omens;
                    playerIP.health -= damageDealt;
                    cout << "Your own violence reflects out of the mirror and cuts you for " << damageDealt << " damage!" << endl;
                } else {
                    cout << "The Oracle hides behind reflected futures." << endl;
                }
            } else if (nameOfAction == "Blind Star") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                int damageDealt = randomNum(12, 16) + omens * 2 + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    damageDealt /= 2;
                    omens -= 2;
                    makeZeroIfNegative(omens);
                    cout << "You crack the blind star's prophecy before it lands." << endl;
                }
                playerIP.health -= damageDealt;
                omens = 0;
                mirrorFate = false;
                cout << "The blind star falls without light, dealing " << damageDealt << " damage and spending the Oracle's Omens." << endl;
            } else if (nameOfAction == "Unask") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit interrupts the impossible question." << endl;
                    mirrorFate = false;
                } else {
                    int healAmount = randomNum(6, 10) + omens;
                    health += healAmount;
                    PS.temporaryDamageModifier -= 1;
                    cout << "The Oracle removes the question of its injury, healing " << healAmount << " health and making your next answer weaker." << endl;
                }
            }
        }
    }
};


class DuneMaw : public Encounter {
private:
    int buried = 0;
    int swallowedSand = 0;
public:
    explicit DuneMaw(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(95, "Dune Maw", {"Glass Teeth", "Undersand Drag", "Burrow", "Sand Geyser", "Relic Belch"}, {25, 25, 20, 20, 10},
        {"The sand opens like a throat, and rows of glass teeth tilt toward you...",
         "Something hooks your shadow from underneath the dune...",
         "The Dune Maw folds itself below the desert skin...",
         "The dune around you trembles with pressure...",
         "Old coins, bones, and broken spearheads rattle in the beast's stomach..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Glass Teeth") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = buried > 0 ? "Quick" : "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(13, 18) + MS.temporaryDamageModifier + swallowedSand - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your critical hit cracks several glass teeth loose before the bite lands!" << endl;
                    damageDealt /= 2;
                    swallowedSand = 0;
                } else if (PAD.type == "PARRY") {
                    cout << "You catch one glass tooth on your blade and lever it sideways." << endl;
                    MS.temporaryDamageModifier -= 1;
                }
                playerIP.health -= damageDealt;
                cout << "The Dune Maw bites across you for " << damageDealt << " damage!" << endl;
                buried = 0;
            } else if (nameOfAction == "Undersand Drag") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(8, 13) + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                if (PAD.type == "CHARGE") {
                    PS.momentum = 0;
                    PS.temporaryDamageModifier -= 2;
                    cout << "It drags you under while you are distracted. Your Momentum is buried with your footing!" << endl;
                }
                cout << "Hooks of packed sand rake your ankles for " << damageDealt << " damage!" << endl;
            } else if (nameOfAction == "Burrow") {
                MAD MAD;
                MAD.defenceValue = 12 + buried * 3;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                buried += 1;
                swallowedSand += 2;
                MS.danger += 1;
                if (PAD.type == "ATTACK" || PAD.type == "MISSED HIT") {
                    PS.momentum -= 1;
                    cout << "Your weapon bites only sand. The Maw learns the rhythm of your steps." << endl;
                } else if (PAD.type == "MAGIC") {
                    buried = 0;
                    cout << "Your magic burns a shape through the dune, forcing the Maw back upward." << endl;
                } else {
                    cout << "The Maw vanishes below the desert and comes back heavier with swallowed sand." << endl;
                }
            } else if (nameOfAction == "Sand Geyser") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(15, 21) + MS.temporaryDamageModifier + MS.danger - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "DODGE") {
                    cout << "You move with the dune just before it erupts." << endl;
                    damageDealt = 0;
                } else if (PAD.type == "CHARGE") {
                    PS.momentum = 0;
                    cout << "The geyser catches you standing still and erases your footing." << endl;
                }
                playerIP.health -= damageDealt;
                cout << "A geyser of burning sand explodes beneath you for " << damageDealt << " damage!" << endl;
                MS.danger = 0;
            } else if (nameOfAction == "Relic Belch") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int n = randomNum(1, 10);
                if (n <= 4) {
                    playerIP.currentScrapMetal += 1;
                    cout << "The Maw belches a usable shard of ancient Scrap Metal at your feet." << endl;
                } else if (n <= 7) {
                    int damageDealt = randomNum(5, 9) - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "A rusted spearhead shoots from its throat and hits you for " << damageDealt << " damage!" << endl;
                } else {
                    health += 10;
                    swallowedSand += 1;
                    cout << "It swallows its own relics again, using them like grinding stones inside its body." << endl;
                }
            }
        }
    }
};

class CrystalMatriarch : public Encounter {
private:
    int shards = 3;
    int resonance = 0;
public:
    explicit CrystalMatriarch(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(105, "Crystal Matriarch", {"Prism Lance", "Mirror Brood", "Resonant Hymn", "Shard Mantle", "Shatter Command"}, {25, 20, 20, 20, 15},
        {"The Matriarch angles her crown, focusing light into a spear...",
         "Small crystal children crawl out of the wall behind her...",
         "The cave begins to sing in the same voice she has...",
         "She folds shattered gems across herself like armor...",
         "Every crystal in the chamber points at you at once..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Prism Lance") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = resonance >= 2 ? "Quick" : "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(12, 17) + resonance + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                if (PAD.type == "DEFENSE") {
                    shards -= 1;
                    cout << "Your guard catches the lance and breaks one of her orbiting shards." << endl;
                }
                cout << "A prism lance punches through the dark for " << damageDealt << " damage!" << endl;
            } else if (nameOfAction == "Mirror Brood") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                shards += 2;
                if (PAD.type == "CRITICAL HIT") {
                    shards -= 2;
                    cout << "Your critical strike scatters the newborn mirror brood before it can stand." << endl;
                } else {
                    cout << "Tiny crystal reflections gather around her ankles. Shards: " << shards << endl;
                }
            } else if (nameOfAction == "Resonant Hymn") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                resonance += 1;
                PS.temporaryDamageModifier -= 1;
                if (PAD.type == "MAGIC") {
                    resonance = 0;
                    cout << "Your magic breaks the note and the whole cave goes briefly silent." << endl;
                } else {
                    cout << "The hymn makes your bones feel like instruments. Your attack weakens." << endl;
                }
            } else if (nameOfAction == "Shard Mantle") {
                MAD MAD;
                MAD.defenceValue = 8 + shards;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                health += 5 + shards;
                if (PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT") {
                    int damageDealt = shards - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The mantle cuts your hands as you strike it for " << damageDealt << " damage." << endl;
                }
                cout << "The Matriarch armors herself in glittering ruin." << endl;
            } else if (nameOfAction == "Shatter Command") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(6, 9) * shards + resonance - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "DODGE") {
                    damageDealt /= 2;
                    cout << "You dodge through the first burst, but fragments still chase you." << endl;
                } else if (PAD.type == "CHARGE") {
                    PS.momentum = 0;
                    cout << "The shatter command catches you planning and turns your opening into panic." << endl;
                }
                playerIP.health -= damageDealt;
                cout << "Her shards detonate outward for " << damageDealt << " damage!" << endl;
                shards = 1;
                resonance = 0;
            }
            if (shards < 0) {
                shards = 0;
            }
        }
    }
};

class TideLeviathan : public Encounter {
private:
    int tide = 1;
    int undertow = 0;
public:
    explicit TideLeviathan(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(120, "Tide Leviathan", {"Coral Crush", "Undertow", "Moon Pull", "Pearl Eye", "Tidal Break"}, {25, 25, 15, 20, 15},
        {"A coral-plated coil rises from the water...",
         "The water around your knees begins to pull backward...",
         "The moon's reflection bends toward the monster's open jaws...",
         "One pearl-white eye opens and remembers every wound in you...",
         "The reef goes silent, just before the wave comes down..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Coral Crush") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(12, 17) + tide + MS.temporaryDamageModifier - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                if (PAD.type == "PARRY") {
                    tide -= 1;
                    cout << "Your parry snaps coral from its jaw and lowers the tide around it." << endl;
                }
                cout << "The Leviathan crushes you against living coral for " << damageDealt << " damage!" << endl;
            } else if (nameOfAction == "Undertow") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Quick";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                undertow += 1;
                PS.momentum -= 2;
                if (PAD.type == "DODGE") {
                    undertow = 0;
                    cout << "You leap with the pull, letting the undertow waste itself." << endl;
                } else {
                    int damageDealt = randomNum(5, 8) + undertow - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    cout << "The current drags you across reef glass for " << damageDealt << " damage." << endl;
                }
            } else if (nameOfAction == "Moon Pull") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                tide += 2;
                MS.danger += 1;
                if (PAD.type == "MAGIC") {
                    tide -= 2;
                    cout << "Your magic muddies the reflection and weakens the moon-pull." << endl;
                } else {
                    cout << "The tide climbs as if the sea itself has been commanded." << endl;
                }
            } else if (nameOfAction == "Pearl Eye") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (PAD.type == "CHARGE") {
                    playerIP.soul -= 2;
                    makeZeroIfNegative(playerIP.soul);
                    cout << "The Pearl Eye sees the thought forming and steals 2 Souls from it." << endl;
                } else if (PAD.type == "CRITICAL HIT") {
                    health -= 8;
                    cout << "You strike the Pearl Eye itself. The Leviathan recoils in ancient pain." << endl;
                } else {
                    MS.temporaryDamageModifier += 1;
                    cout << "The Pearl Eye studies your wounds. Its next strikes grow crueler." << endl;
                }
            } else if (nameOfAction == "Tidal Break") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(15, 22) + tide + undertow + MS.danger - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "DEFENSE") {
                    damageDealt /= 2;
                    cout << "You brace low and let the worst of the wave break above you." << endl;
                }
                playerIP.health -= damageDealt;
                cout << "A full tide falls on you for " << damageDealt << " damage!" << endl;
                tide = 1;
                undertow = 0;
                MS.danger = 0;
            }
            if (tide < 1) {
                tide = 1;
            }
        }
    }
};

class CinderDragon : public Encounter {
private:
    int heat = 0;
    int airborne = 0;
    int hoardGreed = 0;
public:
    explicit CinderDragon(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(165, "Cinder Dragon", {"Claw Through Stone", "Furnace Breath", "Take Wing", "Hoard Stare", "Melt Armor", "Ancient Roar"}, {20, 20, 15, 15, 15, 15},
        {"The dragon drags one claw through the stone like wet clay...",
         "Its ribs glow orange. The cave becomes an oven with teeth...",
         "The Cinder Dragon crouches, wings spreading against the roof...",
         "Its eyes move from you to everything you carry...",
         "A ribbon of white fire coils around your guard...",
         "The mountain remembers why it was afraid of wings..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);
        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Claw Through Stone") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = airborne > 0 ? "Quick" : "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(16, 23) + MS.temporaryDamageModifier + heat - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                if (PAD.type == "PARRY") {
                    heat -= 2;
                    cout << "Your parry sheds sparks from the dragon's claw and cools its rage." << endl;
                }
                cout << "The Cinder Dragon carves through stone and flesh for " << damageDealt << " damage!" << endl;
                airborne = 0;
            } else if (nameOfAction == "Furnace Breath") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                MAD.attackSpeed = "Slow";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                int damageDealt = randomNum(20, 28) + heat + MS.danger - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                if (PAD.type == "DODGE") {
                    damageDealt /= 2;
                    cout << "You escape the center of the breath, but the air itself is burning." << endl;
                } else if (PAD.type == "CHARGE") {
                    PS.momentum = 0;
                    damageDealt += 8;
                    cout << "Thinking inside dragonfire proves almost fatal." << endl;
                }
                playerIP.health -= damageDealt;
                cout << "Furnace breath rolls over you for " << damageDealt << " damage!" << endl;
                heat = 0;
                MS.danger = 0;
            } else if (nameOfAction == "Take Wing") {
                MAD MAD;
                MAD.defenceValue = 10;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                airborne = 1;
                heat += 2;
                if (PAD.type == "CRITICAL HIT" || PAD.type == "MAGIC") {
                    airborne = 0;
                    cout << "You interrupt the takeoff. The dragon crashes back into its hoard." << endl;
                } else {
                    cout << "The dragon lifts into the smoke. Its next strike will come from above." << endl;
                }
            } else if (nameOfAction == "Hoard Stare") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                hoardGreed += 1;
                if (playerIP.currentScrapMetal > 0) {
                    playerIP.currentScrapMetal -= 1;
                    health += 8;
                    cout << "The dragon's stare pulls a piece of Scrap Metal from your pack into the hoard." << endl;
                } else {
                    playerIP.soul -= 1;
                    makeZeroIfNegative(playerIP.soul);
                    cout << "With no metal to claim, it taxes a Soul instead." << endl;
                }
                if (PAD.type == "ATTACK" || PAD.type == "CRITICAL HIT") {
                    cout << "Your aggression keeps it from taking more." << endl;
                    hoardGreed = 0;
                }
            } else if (nameOfAction == "Melt Armor") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                playerIP.hardiness -= 1;
                makeZeroIfNegative(playerIP.hardiness);
                PS.temporaryDamageModifier -= 1;
                int damageDealt = randomNum(8, 12) + heat - PAD.defenceValue;
                makeZeroIfNegative(damageDealt);
                playerIP.health -= damageDealt;
                cout << "White fire softens your guard. You take " << damageDealt << " damage and lose 1 Hardiness." << endl;
            } else if (nameOfAction == "Ancient Roar") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                MS.danger += 2;
                heat += 2;
                PS.momentum -= 2;
                if (PAD.type == "MAGIC") {
                    MS.danger -= 1;
                    cout << "Your magic answers the roar with a smaller, human defiance." << endl;
                } else {
                    cout << "The roar empties your lungs and fills the cave with old terror." << endl;
                }
            }
            if (heat < 0) {
                heat = 0;
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
