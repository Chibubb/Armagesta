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

class CandleWraith : public Encounter {
private:
    int flame = 1;
public:
    explicit CandleWraith(Player& playerData, BackgroundMusicManager& musicManager)
    : Encounter(30, "Candle Wraith", {"Wax Touch", "Soul Flicker", "Melt Away", "Blue Flame"}, {35, 25, 20, 20}
        , {"The Candle Wraith reaches out with a dripping wax hand...",
           "The Candle Wraith's flame leans toward the souls inside your chest...",
           "The Candle Wraith softens into a puddle of glowing wax...",
           "The Candle Wraith's flame turns blue and the room feels suddenly colder..."}, playerData, musicManager) {};

    void doAction() override {
        const int x = getAction();
        const string nameOfAction = getActionName(x);

        printIntent(x);

        if (getHealth() > 0) {
            if (nameOfAction == "Wax Touch") {
                MAD MAD;
                MAD.type = "M-ATTACK";
                MAD.attackSpeed = "Normal";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit blows the flame sideways and ruins the touch!" << endl;
                } else {
                    int damageDealt = randomNum(6, 9) + flame + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.temporaryDamageModifier -= 1;
                    cout << "The Candle Wraith presses hot wax into your arm, dealing " << damageDealt << " damage and weakening your grip!" << endl;
                }
            } else if (nameOfAction == "Soul Flicker") {
                MAD MAD;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "MAGIC") {
                    flame -= 1;
                    makeZeroIfNegative(flame);
                    cout << "Your magic fights the Wraith's flame and dims it!" << endl;
                } else if (playerIP.soul > 0) {
                    playerIP.soul -= 1;
                    flame += 1;
                    cout << "The Candle Wraith steals 1 Soul from you. Its flame grows brighter." << endl;
                } else {
                    int damageDealt = randomNum(5, 7);
                    playerIP.health -= damageDealt;
                    cout << "It finds no Souls to steal, so it burns your life instead for " << damageDealt << " damage!" << endl;
                }
            } else if (nameOfAction == "Melt Away") {
                MAD MAD;
                MAD.defenceValue = 7 + flame;
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit catches the Wraith before it fully melts away!" << endl;
                    flame -= 1;
                    makeZeroIfNegative(flame);
                } else {
                    health += randomNum(3, 5) + flame;
                    cout << "The Candle Wraith melts away from danger, then reforms with renewed wax." << endl;
                }
            } else if (nameOfAction == "Blue Flame") {
                MAD MAD;
                MAD.type = "R-ATTACK";
                PAD PAD = doPlayerTurn_AndGetPlayerActionType(MAD);
                if (health <= 0) return;

                if (PAD.type == "CRITICAL HIT") {
                    cout << "Your Critical Hit scatters the blue flame before it leaves the wick!" << endl;
                } else {
                    int damageDealt = randomNum(8, 11) + flame + MS.temporaryDamageModifier - PAD.defenceValue;
                    makeZeroIfNegative(damageDealt);
                    playerIP.health -= damageDealt;
                    PS.momentum -= 1;
                    makeZeroIfNegative(PS.momentum);
                    cout << "Blue fire crawls across the floor and chills you for " << damageDealt << " damage. You lose 1 Momentum." << endl;
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
