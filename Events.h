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
            encounterAncientEnt(playerIP, musicManager);
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


namespace ArmagestaBiomeTools {
    inline int weightedChoice(const vector<int>& chances) {
        int totalChance = 0;
        for (const int chance : chances) {
            totalChance += chance;
        }
        int roll = randomNum(1, totalChance);
        int cumulativeChance = 0;
        for (int i = 0; i < chances.size(); i++) {
            cumulativeChance += chances[i];
            if (roll <= cumulativeChance) {
                return i;
            }
        }
        return 0;
    }

    inline int askChoice(const vector<string>& choices) {
        if (choices.size() == 1) {
            return 0;
        }

        cout << "Choose one:" << endl;
        for (int i = 0; i < choices.size(); i++) {
            cout << i + 1 << ": " << choices[i] << endl;
        }
        cout << endl;

        string triedChoice;
        while (true) {
            getline(cin >> ws, triedChoice);
            for (int i = 0; i < choices.size(); i++) {
                if (triedChoice == choices[i] || triedChoice == to_string(i + 1)) {
                    return i;
                }
            }
            cout << "Invalid input, try again..." << endl;
        }
    }

    inline bool hasCombatAction(const Player& playerIP, const string& actionName) {
        for (const auto& action : playerIP.combatActions) {
            if (action == actionName) {
                return true;
            }
        }
        return false;
    }

    inline void unlockCombatAction(Player& playerIP, const string& actionName, const string& description) {
        if (hasCombatAction(playerIP, actionName)) {
            cout << "You already know " << actionName << ". The lesson echoes, but does not change you a second time." << endl;
            return;
        }
        playerIP.combatActions.emplace_back(actionName);
        playerIP.combatActionsDescriptions.emplace_back(description);
        cout << "NEW COMBAT ACTION UNLOCKED: " << actionName << endl;
        cout << description << endl;
    }

    inline void hurtPlayer(Player& playerIP, const int damage) {
        int trueDamage = damage;
        makeZeroIfNegative(trueDamage);
        playerIP.health -= trueDamage;
        makeZeroIfNegative(playerIP.health);
    }

    inline void printEventHeader(const string& eventName, const string& eventFlavorText) {
        cout << endl << "===== " << eventName << " =====" << endl;
        cout << eventFlavorText << endl << endl;
    }
}

class Desert : public BiomeEventDatabase {
private:
    vector<int> eventChances = {25, 20, 15, 15, 10, 10, 5};
    vector<string> eventNames = {"Sun-Buried Road", "Obsidian Caravan", "Mirage Court", "Dune Maw Sign", "Buried Saint", "Storm of Singing Sand", "Black Pyramid Far Away"};
    vector<string> eventFlavorText = {
        "A road of half-buried white stones cuts through the dunes. Every stone is engraved with the same warning: DO NOT TRUST SHADE.",
        "A caravan of black wagons rolls without horses. Its wheels turn over sand without leaving tracks, and the merchants wave like they expected you.",
        "A palace of heat and blue silk trembles in the distance. Inside its open gates, your own reflection sits on a throne of water.",
        "The dune ahead rises and falls like something asleep beneath a blanket. Broken shields circle it like teeth around a mouth.",
        "A stone hand reaches from the sand, still clutching a sun-shaped ring. The rest of the saint is buried below centuries of thirst.",
        "The wind begins to sing with hundreds of dry voices. Some are begging. Some are laughing. One is yours.",
        "For one moment the horizon clears, revealing a black pyramid where no map says anything should stand. Then the heat closes again."
    };
    vector<vector<string>> eventChoices = {
        {"Follow the Road", "Dig Under a Milestone", "Hide in the Shade", "Turn Back"},
        {"Trade Scrap", "Ask About the Old War", "Rob the Wagon", "Travel With Them"},
        {"Bow to the Reflection", "Challenge the Reflection", "Break the Mirror Water", "Leave Thirsty"},
        {"Step Softly", "Throw Scrap Metal", "Fight the Maw", "Sprint Across"},
        {"Read the Epitaph", "Take the Sun-Ring", "Pour Souls Into the Grave", "Move On"},
        {"Cover Your Eyes", "Walk Into the Song", "Anchor Yourself", "Sing Back"},
        {"Mark it on Your Map", "Kneel to the Horizon", "Ignore the Omen"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }

    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Follow the Road") {
            cout << "The stones are warm through your boots. The road bends around every dangerous dune as if remembering where people died." << endl;
            playerIP.permanentMomentum += 1;
            cout << "You gained 1 permanent Momentum." << endl;
        } else if (ChoiceName == "Dig Under a Milestone") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                cout << "You find a pocket of old coins and one usable piece of Scrap Metal." << endl;
                playerIP.currentScrapMetal += 1;
            } else if (n <= 8) {
                cout << "A Thorn Imp was sleeping under the stone, wrapped in roots that should not grow here!" << endl;
                encounterThornImp(playerIP, musicManager);
            } else {
                cout << "You uncover a buried prayer. It does not feed you, but it steadies your soul." << endl;
                playerIP.maxSoul += 1;
                playerIP.gainSoul(2);
            }
        } else if (ChoiceName == "Hide in the Shade") {
            cout << "The warning was true. The shade is colder than night and bites into your thoughts." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
            playerIP.soul -= 1;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Turn Back") {
            cout << "You leave the road alone. Sometimes wisdom feels exactly like cowardice." << endl;
            playerIP.heal(4);
        } else if (ChoiceName == "Trade Scrap") {
            if (playerIP.currentScrapMetal > 0) {
                playerIP.currentScrapMetal -= 1;
                playerIP.maxHealth += 4;
                playerIP.heal(12);
                cout << "The merchant accepts your metal and gives you a waterskin stitched from something holy. Max Health increased by 4." << endl;
            } else {
                cout << "You have no Scrap Metal. The merchant sells you a look of pity for free." << endl;
            }
        } else if (ChoiceName == "Ask About the Old War") {
            cout << "They tell you the desert used to be a kingdom of glass. The Citadel betrayed it, and the sun finished the execution." << endl;
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Rob the Wagon") {
            const int n = randomNum(1, 10);
            if (n <= 6) {
                cout << "The wagon is guarded by an Iron Scarab hiding inside a coin chest!" << endl;
                encounterIronScarab(playerIP, musicManager);
                playerIP.currentScrapMetal += 2;
                cout << "You steal 2 Scrap Metal after the fight." << endl;
            } else {
                cout << "The merchants were only mirages. Your hands close around hot air until your skin blisters." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, 10);
            }
        } else if (ChoiceName == "Travel With Them") {
            cout << "They let you walk in the wheel-shadow. By dusk, you know how to conserve every movement." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Bow to the Reflection") {
            cout << "Your reflection smiles with your mouth and grants you a lesson in humility." << endl;
            playerIP.hardiness += 1;
            playerIP.soul -= 1;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Challenge the Reflection") {
            cout << "The reflection steps out wrong-handed and attacks with your almost-face." << endl;
            encounterStarlessOracle(playerIP, musicManager);
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Break the Mirror Water") {
            cout << "The palace collapses into one real drop of water. You drink it and remember rain." << endl;
            playerIP.maxSoul += 1;
            playerIP.heal(15);
        } else if (ChoiceName == "Leave Thirsty") {
            cout << "You leave wanting. Want is useful. It points forward." << endl;
            playerIP.gainSoul(1);
        } else if (ChoiceName == "Step Softly") {
            cout << "You cross by matching your breath to the dune's sleeping rhythm." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Throw Scrap Metal") {
            if (playerIP.currentScrapMetal > 0) {
                playerIP.currentScrapMetal -= 1;
                cout << "The metal vanishes into the sand. The thing below chews contentedly and lets you pass." << endl;
            } else {
                cout << "You throw nothing. The dune notices the insult." << endl;
                encounterDuneMaw(playerIP, musicManager);
            }
        } else if (ChoiceName == "Fight the Maw") {
            cout << "You stamp on the dune until the desert opens its mouth." << endl;
            encounterDuneMaw(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.maxSoul += 1;
                cout << "Inside the corpse you find swallowed prayers. Max Soul increased by 1." << endl;
            }
        } else if (ChoiceName == "Sprint Across") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                cout << "You sprint faster than the sand can think." << endl;
                playerIP.permanentMomentum += 1;
            } else {
                cout << "The sand guesses correctly." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, randomNum(12, 18));
            }
        } else if (ChoiceName == "Read the Epitaph") {
            cout << "The epitaph says: Here Lies the First Person Who Mistook Survival for Victory." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Take the Sun-Ring") {
            cout << "The saint's hand closes when you touch the ring. A Lantern Bearer rises from the heat to judge the theft." << endl;
            encounterLanternBearer(playerIP, musicManager);
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Pour Souls Into the Grave") {
            if (playerIP.soul >= 3) {
                playerIP.soul -= 3;
                playerIP.maxHealth += 8;
                playerIP.heal(20);
                cout << "The saint sighs under the sand. Something blessed climbs into your blood. Max Health increased by 8." << endl;
            } else {
                cout << "You do not have enough Souls to wake anything kind." << endl;
            }
        } else if (ChoiceName == "Move On") {
            cout << "You leave the dead their jewelry and their silence." << endl;
            playerIP.heal(3);
        } else if (ChoiceName == "Cover Your Eyes") {
            cout << "Blind, you trust your feet. The storm passes over you like a beast sniffing a stone." << endl;
            playerIP.hardiness += 1;
        } else if (ChoiceName == "Walk Into the Song") {
            cout << "The voices teach you a forbidden marching rhythm, then demand payment." << endl;
            playerIP.permanentMomentum += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 12);
        } else if (ChoiceName == "Anchor Yourself") {
            cout << "You drive your weapon into the sand. When the storm clears, glass has formed around the blade's edge." << endl;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Sing Back") {
            cout << "The storm hates your living voice. It sends an Echo Bat made of sand-sound at you." << endl;
            encounterEchoBat(playerIP, musicManager);
            playerIP.gainSoul(3);
        } else if (ChoiceName == "Mark it on Your Map") {
            cout << "You scratch the pyramid's direction into memory. A place like that is not found by walking. It is found by being ready." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Kneel to the Horizon") {
            cout << "Something inside the pyramid kneels back. That is much worse." << endl;
            encounterStarlessOracle(playerIP, musicManager);
        } else if (ChoiceName == "Ignore the Omen") {
            cout << "You look away. The desert approves of people who can choose not to know everything." << endl;
            playerIP.heal(8);
        }
    }

public:
    explicit Desert(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = Desert::getChoice();
        Desert::doEvent();
    }
};

class Caves : public BiomeEventDatabase {
private:
    vector<int> eventChances = {20, 18, 17, 15, 12, 10, 8};
    vector<string> eventNames = {"Mouth of the Earth", "Blind River", "Fossil Gate", "Crystal Choir", "Deep Nest", "Old Lift", "Underforge"};
    vector<string> eventFlavorText = {
        "The cave entrance exhales mineral-cold air. It smells like coins, rain, and something that has never seen the sun.",
        "A black river moves without reflecting anything. Small pale fish swim upstream with human teeth.",
        "The tunnel ends at a gate made from the ribcage of a creature large enough to make mountains feel young.",
        "Crystals hang from the ceiling in perfect rows. When you breathe, they answer in harmony.",
        "Webbing thick as rope knots the walls. You can hear many legs stopping whenever you stop.",
        "A wooden lift hangs over a vertical shaft. Its rope is newer than the cave around it, which is not comforting.",
        "An ancient forge glows under the stone, still hot without coal. Hammers strike somewhere below, but no hands are visible."
    };
    vector<vector<string>> eventChoices = {
        {"Light a Torch", "Enter Quietly", "Call Into the Dark", "Take a Wall Marking"},
        {"Drink the Water", "Follow the River", "Catch a Pale Fish", "Cross Carefully"},
        {"Open the Gate", "Study the Bones", "Leave an Offering", "Break a Rib"},
        {"Listen", "Sing With Them", "Harvest a Crystal", "Shatter the Choir"},
        {"Burn the Webs", "Move Slowly", "Pull the Egg Sac", "Retreat"},
        {"Ride Down", "Cut the Rope", "Repair the Brakes", "Climb Around"},
        {"Take a Hammer", "Quench Your Blade", "Wake the Forge", "Leave the Heat"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Light a Torch") {
            cout << "The light reveals old arrows pointed deeper inward. Someone wanted future travelers to continue." << endl;
            playerIP.gainSoul(1);
        } else if (ChoiceName == "Enter Quietly") {
            cout << "You move quietly enough to hear the cave decide not to eat you today." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Call Into the Dark") {
            cout << "Your voice returns wearing wings." << endl;
            encounterEchoBat(playerIP, musicManager);
        } else if (ChoiceName == "Take a Wall Marking") {
            cout << "The marking flakes off into your palm as bright ore." << endl;
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Drink the Water") {
            const int n = randomNum(1, 10);
            if (n <= 6) {
                cout << "The water tastes like stone and heals like sleep." << endl;
                playerIP.heal(14);
            } else {
                cout << "The water remembers drowning. It pulls breath from you." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, 10);
                playerIP.maxSoul += 1;
            }
        } else if (ChoiceName == "Follow the River") {
            cout << "The river leads you to a lost shrine scratched with maps of the underworld." << endl;
            playerIP.maxSoul += 1;
            playerIP.gainSoul(3);
        } else if (ChoiceName == "Catch a Pale Fish") {
            cout << "The fish bites back with a full set of tiny human teeth." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 4);
            playerIP.heal(10);
            cout << "It is still nourishing, which makes everything worse." << endl;
        } else if (ChoiceName == "Cross Carefully") {
            cout << "You cross stone by stone. Halfway over, a Glass Spider tests your patience." << endl;
            if (randomNum(1, 10) <= 5) {
                encounterGlassSpider(playerIP, musicManager);
            } else {
                playerIP.permanentMomentum += 1;
            }
        } else if (ChoiceName == "Open the Gate") {
            cout << "The Fossil Gate opens because it still recognizes blood as a key." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 6);
            playerIP.maxSoul += 2;
        } else if (ChoiceName == "Study the Bones") {
            cout << "You learn where even giants were weak." << endl;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Leave an Offering") {
            if (playerIP.soul >= 2) {
                playerIP.soul -= 2;
                cout << "The gate accepts the Souls and opens a small safe path." << endl;
                playerIP.heal(10);
            } else {
                cout << "You have too few Souls to impress dead anatomy." << endl;
            }
        } else if (ChoiceName == "Break a Rib") {
            cout << "The rib breaks. The cave immediately tries to correct your disrespect." << endl;
            encounterGraveMoss(playerIP, musicManager);
            playerIP.currentScrapMetal += 2;
        } else if (ChoiceName == "Listen") {
            cout << "The choir sings the exact shape of your fear until it becomes smaller." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Sing With Them") {
            cout << "Your voice joins the crystal architecture. For one breath, you are harder to break." << endl;
            playerIP.hardiness += 2;
        } else if (ChoiceName == "Harvest a Crystal") {
            cout << "The choir does not enjoy being harvested." << endl;
            encounterCrystalMatriarch(playerIP, musicManager);
            playerIP.currentScrapMetal += 2;
        } else if (ChoiceName == "Shatter the Choir") {
            cout << "The sound dies. The silence afterward crawls into your weapon." << endl;
            playerIP.permanentDamageModifier += 2;
            playerIP.soul -= 2;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Burn the Webs") {
            cout << "The webbing burns blue. Things inside it wake up angry and already on fire." << endl;
            encounterGlassSpider(playerIP, musicManager);
            if (randomNum(1, 10) <= 4) {
                encounterGlassSpider(playerIP, musicManager);
            }
        } else if (ChoiceName == "Move Slowly") {
            cout << "You move like dust settling. The nest lets you pass as if you are not worth counting." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Pull the Egg Sac") {
            cout << "Inside the sac is not an egg, but a lantern that has been waiting to hatch." << endl;
            encounterLanternBearer(playerIP, musicManager);
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Retreat") {
            cout << "Backing away from too many legs is one of the oldest forms of intelligence." << endl;
            playerIP.heal(5);
        } else if (ChoiceName == "Ride Down") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                cout << "The lift screams downward and somehow survives. So do you." << endl;
                playerIP.maxSoul += 1;
            } else {
                cout << "The lift drops too fast. You arrive bruised and educated." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, randomNum(12, 20));
                playerIP.permanentMomentum += 1;
            }
        } else if (ChoiceName == "Cut the Rope") {
            cout << "The lift falls into darkness for a very long time. Something below applauds." << endl;
            encounterSiltHydra(playerIP, musicManager);
        } else if (ChoiceName == "Repair the Brakes") {
            if (playerIP.currentScrapMetal >= 1) {
                playerIP.currentScrapMetal -= 1;
                cout << "You spend 1 Scrap Metal and make the lift safer. Practical courage is still courage." << endl;
                playerIP.hardiness += 1;
            } else {
                cout << "You have no Scrap Metal. The brakes remain more suggestion than mechanism." << endl;
            }
        } else if (ChoiceName == "Climb Around") {
            cout << "Your fingers find impossible holds. You climb around the shaft and come away stronger." << endl;
            playerIP.permanentMomentum += 1;
            playerIP.maxHealth += 3;
        } else if (ChoiceName == "Take a Hammer") {
            cout << "The hammer weighs more than its size. It teaches your arm one brutal lesson." << endl;
            playerIP.permanentDamageModifier += 1;
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Quench Your Blade") {
            cout << "You cool your weapon in black water. Steam rises in the shape of screaming kings." << endl;
            playerIP.permanentDamageModifier += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 5);
        } else if (ChoiceName == "Wake the Forge") {
            cout << "The forge wakes as a Reforged Knight, still wearing the armor it was made to repair." << endl;
            encounterReforgedKnight(playerIP, musicManager);
            playerIP.currentScrapMetal += 3;
        } else if (ChoiceName == "Leave the Heat") {
            cout << "You leave the ancient work unfinished. Not every sleeping thing needs a morning." << endl;
            playerIP.heal(6);
        }
    }

public:
    explicit Caves(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = Caves::getChoice();
        Caves::doEvent();
    }
};

class OldRedwoods : public BiomeEventDatabase {
private:
    vector<int> eventChances = {22, 18, 16, 14, 12, 10, 8};
    vector<string> eventNames = {"High Canopy", "Bark Tribunal", "Amber Rain", "War-Scar Grove", "Nursery of Small Ents", "Hanging Bells", "The Red Root"};
    vector<string> eventFlavorText = {
        "The redwoods rise so high that the sky feels like a rumor told by leaves.",
        "Three trees lean inward, their bark twisted into faces. They do not speak, but the roots around your feet tighten like a courtroom.",
        "Golden amber drips from wounds in the branches. Inside each drop, something tiny is preserved mid-scream.",
        "A circle of redwoods bears axe scars wider than doors. The forest remembers the war and has not forgiven the metal that made it.",
        "Young ents no taller than you pretend to be ordinary saplings. They are terrible actors.",
        "Thousands of wooden bells hang from threads of moss. None move, but all of them ring inside your chest.",
        "A single red root crosses the path, pulsing like a vein from one end of the forest to the other."
    };
    vector<vector<string>> eventChoices = {
        {"Climb Toward the Sky", "Search the Hollow", "Sleep Between Roots", "Cut a Vine Bridge"},
        {"Confess Violence", "Offer Scrap Metal", "Argue Your Case", "Refuse Judgment"},
        {"Collect Amber", "Break an Amber Prison", "Taste the Sap", "Shelter Under Leaves"},
        {"Touch the Scars", "Sharpen Your Blade There", "Apologize to the Grove", "Challenge the Memory"},
        {"Play Along", "Ask for Their Elder", "Steal a Tiny Branch", "Teach Them a Human Word"},
        {"Ring One Bell", "Cut Down the Bells", "Listen Without Moving", "Leave Quietly"},
        {"Follow the Root", "Step Over It", "Cut the Root", "Feed it Souls"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Climb Toward the Sky") {
            const int n = randomNum(1, 10);
            if (n <= 6) {
                cout << "Above the fog, you see the world arranged like a wound around the Citadel." << endl;
                playerIP.printMap();
                playerIP.maxSoul += 1;
            } else {
                cout << "A branch shifts. The fall is short by redwood standards and horrifying by yours." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, randomNum(10, 18));
            }
        } else if (ChoiceName == "Search the Hollow") {
            cout << "Inside the hollow is a shrine of feathers, old teeth, and one useful piece of Scrap Metal." << endl;
            playerIP.currentScrapMetal += 1;
            if (randomNum(1, 10) <= 4) {
                cout << "A Grave Moss colony objects to your searching." << endl;
                encounterGraveMoss(playerIP, musicManager);
            }
        } else if (ChoiceName == "Sleep Between Roots") {
            cout << "You sleep with your back against something older than language." << endl;
            playerIP.heal(18);
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Cut a Vine Bridge") {
            cout << "The bridge snaps. Something on the other side falls before you do." << endl;
            playerIP.permanentDamageModifier += 1;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 7);
        } else if (ChoiceName == "Confess Violence") {
            cout << "The trees accept honesty better than innocence. They reduce your guilt to a lesson." << endl;
            playerIP.hardiness += 1;
            playerIP.soul -= 1;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Offer Scrap Metal") {
            if (playerIP.currentScrapMetal > 0) {
                playerIP.currentScrapMetal -= 1;
                cout << "The roots drag the metal underground like poison removed from a wound." << endl;
                playerIP.maxHealth += 5;
                playerIP.heal(12);
            } else {
                cout << "You offer empty hands. The tribunal considers that almost charming." << endl;
                playerIP.heal(4);
            }
        } else if (ChoiceName == "Argue Your Case") {
            cout << "You argue that humans are brief, frightened, and occasionally useful. The trees find this persuasive." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Refuse Judgment") {
            cout << "The tribunal calls an Ancient Ent to deliver the verdict by hand." << endl;
            encounterAncientEnt(playerIP, musicManager);
        } else if (ChoiceName == "Collect Amber") {
            cout << "The amber hardens around your knuckles, making your guard heavier." << endl;
            playerIP.hardiness += 1;
        } else if (ChoiceName == "Break an Amber Prison") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                cout << "A trapped moth-soul escapes and thanks you by burning warmly in your chest." << endl;
                playerIP.maxSoul += 1;
                playerIP.gainSoul(3);
            } else {
                cout << "You release something that was imprisoned for a reason." << endl;
                encounterThornImp(playerIP, musicManager);
            }
        } else if (ChoiceName == "Taste the Sap") {
            cout << "The sap is bitter and ancient. It thickens your blood with stubborn life." << endl;
            playerIP.maxHealth += 4;
            playerIP.heal(10);
        } else if (ChoiceName == "Shelter Under Leaves") {
            cout << "The leaves close over you like a roof. Rain hits everywhere except your skin." << endl;
            playerIP.heal(8);
        } else if (ChoiceName == "Touch the Scars") {
            cout << "The scars show you a war from the tree's side: axes flashing like insects with purpose." << endl;
            playerIP.permanentDamageModifier += 1;
            playerIP.soul -= 1;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Sharpen Your Blade There") {
            cout << "This is a very bad idea. The grove sends a Hollow Squire made of old battlefield guilt." << endl;
            encounterHollowSquire(playerIP, musicManager);
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Apologize to the Grove") {
            cout << "The apology is accepted by one tree and debated by all the others." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Challenge the Memory") {
            cout << "The memory becomes a knight-shaped absence swinging a rusted blade." << endl;
            encounterReforgedKnight(playerIP, musicManager);
        } else if (ChoiceName == "Play Along") {
            cout << "You pretend not to see the small ents. They pretend not to giggle. Everyone benefits." << endl;
            playerIP.heal(12);
        } else if (ChoiceName == "Ask for Their Elder") {
            cout << "The small ents point in seven different directions, then all whisper: Oldest." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Steal a Tiny Branch") {
            cout << "The branch screams for its mother. The mother arrives faster than expected." << endl;
            encounterAncientEnt(playerIP, musicManager);
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Teach Them a Human Word") {
            cout << "You teach them the word 'maybe.' The forest becomes slightly less certain and slightly less dangerous." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Ring One Bell") {
            cout << "One bell rings. Somewhere in the Citadel, something rings back." << endl;
            playerIP.gainSoul(4);
        } else if (ChoiceName == "Cut Down the Bells") {
            cout << "The bells fall silent in the worst possible way." << endl;
            encounterEchoBat(playerIP, musicManager);
            encounterEchoBat(playerIP, musicManager);
        } else if (ChoiceName == "Listen Without Moving") {
            cout << "You listen until the bells teach you how to be still without being helpless." << endl;
            playerIP.hardiness += 1;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Leave Quietly") {
            cout << "The bells do not ring for cowards. They ring for survivors later." << endl;
            playerIP.heal(5);
        } else if (ChoiceName == "Follow the Root") {
            cout << "The root leads toward the heart of the old forest. You feel the Oldest Redwood watching." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Step Over It") {
            cout << "The root twitches like you stepped over a sleeping giant's finger." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Cut the Root") {
            cout << "Sap sprays like blood. The forest's patience ends." << endl;
            encounterAncientEnt(playerIP, musicManager);
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
        } else if (ChoiceName == "Feed it Souls") {
            if (playerIP.soul >= 4) {
                playerIP.soul -= 4;
                cout << "The root drinks and pulses kindly. Your skin toughens like bark." << endl;
                playerIP.hardiness += 2;
                playerIP.maxHealth += 5;
            } else {
                cout << "The root sniffs your soul and finds the bowl too empty." << endl;
            }
        }
    }

public:
    explicit OldRedwoods(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = OldRedwoods::getChoice();
        OldRedwoods::doEvent();
    }
};

class Swamp : public BiomeEventDatabase {
private:
    vector<int> eventChances = {22, 18, 16, 14, 12, 10, 8};
    vector<string> eventNames = {"Rotwater Ferry", "Mosquito Chapel", "Witchlights", "Silt Hydra Pool", "Drowned Battlefield", "Black Lotus", "Listening Bog"};
    vector<string> eventFlavorText = {
        "A ferry waits in brown water with no ferryman. The pole moves by itself, tapping impatiently against the mud.",
        "A chapel sinks at an angle into the swamp. Mosquitoes gather around its steeple like worshippers.",
        "Blue lights bob between the trees. They move like lanterns carried by people who do not want to be seen.",
        "A still pool bubbles once. Then twice. Then in a rhythm too much like breathing.",
        "Rusty helmets and broken banners poke from the mud. The battle here never ended; it merely became wetter.",
        "A black lotus blooms in water so dark it reflects your face older than it is.",
        "The mud repeats your footsteps after you stop moving. Something below is learning you."
    };
    vector<vector<string>> eventChoices = {
        {"Board the Ferry", "Push it Away", "Pay with a Soul", "Swim Across"},
        {"Enter the Chapel", "Swat the Swarm", "Pray Through the Buzzing", "Loot the Bell"},
        {"Follow the Lights", "Extinguish One", "Call Out", "Mark the Trees"},
        {"Throw a Stone", "Drink from the Pool", "Fight What Rises", "Back Away Slowly"},
        {"Raise a Banner", "Search the Mud", "Challenge the Dead", "Bury a Helmet"},
        {"Pick the Lotus", "Smell the Lotus", "Leave it Blooming", "Feed it Blood"},
        {"Stand Still", "Run", "Speak Your Name", "Stab the Mud"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Board the Ferry") {
            cout << "The ferry carries you across without asking where you are going. That is not kindness; it already knows." << endl;
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Push it Away") {
            cout << "The ferry drifts off offended. The water rises around your boots in response." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 6);
        } else if (ChoiceName == "Pay with a Soul") {
            if (playerIP.soul >= 1) {
                playerIP.soul -= 1;
                cout << "The ferry accepts the Soul and leaves behind a coin-shaped blessing under your tongue." << endl;
                playerIP.maxSoul += 1;
            } else {
                cout << "You have no Soul to pay. The ferry takes skin instead." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
            }
        } else if (ChoiceName == "Swim Across") {
            cout << "This is exactly what the leeches were hoping for." << endl;
            encounterMireLeech(playerIP, musicManager);
            ArmagestaBiomeTools::hurtPlayer(playerIP, 5);
        } else if (ChoiceName == "Enter the Chapel") {
            cout << "Inside, the pews are wet and every hymn book is full of names crossed out." << endl;
            if (randomNum(1, 10) <= 6) {
                encounterGraveMoss(playerIP, musicManager);
            }
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Swat the Swarm") {
            cout << "You kill dozens. Thousands take it personally." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, randomNum(7, 13));
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Pray Through the Buzzing") {
            cout << "Your prayer is ugly, distracted, and apparently honest enough." << endl;
            playerIP.heal(16);
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Loot the Bell") {
            cout << "The chapel bell is guarded by a Lantern Bearer with swamp water in its flame." << endl;
            encounterLanternBearer(playerIP, musicManager);
            playerIP.currentScrapMetal += 2;
        } else if (ChoiceName == "Follow the Lights") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                cout << "The lights lead to dry ground and a hidden stash of medicine." << endl;
                playerIP.heal(20);
            } else {
                cout << "The lights lead to a coven of hanging puppets." << endl;
                encounterMarionetteCoven(playerIP, musicManager);
            }
        } else if (ChoiceName == "Extinguish One") {
            cout << "The light screams. A Candle Wraith would have been better; instead the whole swamp notices." << endl;
            encounterStarlessOracle(playerIP, musicManager);
        } else if (ChoiceName == "Call Out") {
            cout << "Something answers in your own voice, but older and under the water." << endl;
            playerIP.maxSoul += 1;
            playerIP.soul -= 1;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Mark the Trees") {
            cout << "You mark the trees with mud and memory. The swamp becomes slightly less impossible." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Throw a Stone") {
            cout << "The pool throws something back." << endl;
            if (randomNum(1, 10) <= 6) {
                encounterSiltHydra(playerIP, musicManager);
            } else {
                playerIP.currentScrapMetal += 1;
                cout << "It was old Scrap Metal, polished smooth by stomach acid." << endl;
            }
        } else if (ChoiceName == "Drink from the Pool") {
            cout << "The water tastes like teeth and prophecy." << endl;
            playerIP.maxSoul += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 10);
        } else if (ChoiceName == "Fight What Rises") {
            cout << "Several heads rise, each wearing a different expression of hunger." << endl;
            encounterSiltHydra(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.permanentDamageModifier += 1;
            }
        } else if (ChoiceName == "Back Away Slowly") {
            cout << "The pool respects slow fear." << endl;
            playerIP.heal(5);
        } else if (ChoiceName == "Raise a Banner") {
            cout << "The banner pulls a dead army's attention through the mud." << endl;
            encounterSkeleton(playerIP, musicManager);
            encounterHollowSquire(playerIP, musicManager);
        } else if (ChoiceName == "Search the Mud") {
            cout << "You find a shield boss, a finger bone, and usable Scrap Metal." << endl;
            playerIP.currentScrapMetal += randomNum(1, 3);
        } else if (ChoiceName == "Challenge the Dead") {
            cout << "The dead send their champion, because even ghosts enjoy delegation." << endl;
            encounterReforgedKnight(playerIP, musicManager);
        } else if (ChoiceName == "Bury a Helmet") {
            cout << "You give one soldier an ending. The swamp gives you a little peace." << endl;
            playerIP.maxSoul += 1;
            playerIP.heal(10);
        } else if (ChoiceName == "Pick the Lotus") {
            cout << "The lotus stains your fingers black. Its poison becomes medicine if you survive it." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
            playerIP.maxHealth += 6;
        } else if (ChoiceName == "Smell the Lotus") {
            cout << "You dream while standing. In the dream, a Witch's Hut waits for you on chicken-bone stilts." << endl;
            playerIP.gainSoul(4);
        } else if (ChoiceName == "Leave it Blooming") {
            cout << "You let the rare thing live. The swamp, for once, does not punish mercy." << endl;
            playerIP.heal(12);
        } else if (ChoiceName == "Feed it Blood") {
            cout << "The flower drinks from your palm and whispers a hex back into your blood." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 6);
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Stand Still") {
            cout << "The mud copies you perfectly, then gets bored." << endl;
            playerIP.hardiness += 1;
        } else if (ChoiceName == "Run") {
            cout << "The bog loves panic. It follows you like a second pair of legs." << endl;
            encounterMireLeech(playerIP, musicManager);
        } else if (ChoiceName == "Speak Your Name") {
            cout << "The bog repeats your name with affection. That might be worse than hatred." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Stab the Mud") {
            cout << "You wound the ground. The ground bleeds upward." << endl;
            encounterGraveMoss(playerIP, musicManager);
            playerIP.permanentDamageModifier += 1;
        }
    }

public:
    explicit Swamp(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = Swamp::getChoice();
        Swamp::doEvent();
    }
};

class Citadel : public BiomeEventDatabase {
private:
    vector<int> eventChances = {22, 18, 16, 14, 12, 10, 8};
    vector<string> eventNames = {"Lower Gate", "Empty Barracks", "Hall of Petitions", "Prison of Names", "Furnace Chapel", "Gargoyle Eaves", "Royal Armory"};
    vector<string> eventFlavorText = {
        "The Citadel gate is raised just enough for one person to crawl beneath it. That feels less like welcome and more like digestion.",
        "Rows of bunks wait in perfect order. Every pillow has a sword laid on it, like the soldiers left during the same dream.",
        "A marble hall stretches forward, lined with statues of citizens begging a king whose face has been chiseled away.",
        "Iron cells contain no prisoners, only names carved into the walls. Some names are still breathing.",
        "A chapel furnace glows beneath a cracked altar. Here, faith and metal were apparently the same craft.",
        "Stone gargoyles crouch above the walkway, all facing inward toward the throne instead of outward toward enemies.",
        "Weapon racks cover the room, but most blades are fused to the stone by royal decree. One rack still trembles."
    };
    vector<vector<string>> eventChoices = {
        {"Crawl Under", "Lift the Gate", "Knock Politely", "Look for a Side Door"},
        {"Take a Sword", "Read the Orders", "Sleep on a Bunk", "Kick Over a Rack"},
        {"Hear the Petitions", "Smash a Statue", "Answer as King", "Walk the Center Line"},
        {"Read a Name", "Scratch Out a Name", "Give Your Own Name", "Open a Cell"},
        {"Pray to the Furnace", "Steal the Coals", "Repair the Altar", "Wake the Knight"},
        {"Walk Beneath Them", "Throw a Stone", "Compliment the Stonework", "Climb to the Roof"},
        {"Pull the Trembling Blade", "Take Armor Plates", "Melt Scrap Into Edge", "Leave the Weapons"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Crawl Under") {
            cout << "You crawl through old blood and older dust. The gate teeth scrape your back, but not deeply." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 3);
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Lift the Gate") {
            cout << "The gate lifts for exactly one breath, impressed by your audacity." << endl;
            playerIP.hardiness += 1;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
        } else if (ChoiceName == "Knock Politely") {
            cout << "A Hollow Squire opens the gate from the other side and attacks you for your manners." << endl;
            encounterHollowSquire(playerIP, musicManager);
        } else if (ChoiceName == "Look for a Side Door") {
            cout << "You find a servant's entrance marked: for everyone the crown needs but refuses to see." << endl;
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Take a Sword") {
            cout << "The sword is brittle, but its lesson is sharp." << endl;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Read the Orders") {
            cout << "The orders say the Citadel was commanded to survive even if the kingdom did not." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Sleep on a Bunk") {
            cout << "You dream of marching in armor that has no person inside it." << endl;
            playerIP.heal(10);
            if (randomNum(1, 10) <= 4) {
                encounterReforgedKnight(playerIP, musicManager);
            }
        } else if (ChoiceName == "Kick Over a Rack") {
            cout << "The crash wakes the room's discipline." << endl;
            encounterSkeleton(playerIP, musicManager);
            encounterHollowSquire(playerIP, musicManager);
        } else if (ChoiceName == "Hear the Petitions") {
            cout << "The statues whisper problems no king solved. Listening makes you heavier but harder to move." << endl;
            playerIP.hardiness += 2;
            playerIP.soul -= 1;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Smash a Statue") {
            cout << "The marble citizen breaks open. There is a Soul curled inside like a pearl." << endl;
            playerIP.gainSoul(4);
            if (randomNum(1, 10) <= 5) {
                encounterCathedralGargoyle(playerIP, musicManager);
            }
        } else if (ChoiceName == "Answer as King") {
            cout << "For one dangerous second, the hall believes you." << endl;
            playerIP.maxSoul += 2;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Walk the Center Line") {
            cout << "Every statue turns its empty eyes to you. You do not look away." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Read a Name") {
            cout << "The name crawls from the wall into your mouth. You spit it out as a Soul." << endl;
            playerIP.gainSoul(3);
        } else if (ChoiceName == "Scratch Out a Name") {
            cout << "The name thanks you by forgetting how to suffer." << endl;
            playerIP.maxSoul += 1;
            playerIP.heal(8);
        } else if (ChoiceName == "Give Your Own Name") {
            cout << "The prison accepts your name but not all of it. You feel a little less easy to kill and a little less yourself." << endl;
            playerIP.maxHealth += 10;
            playerIP.soul -= 2;
            makeZeroIfNegative(playerIP.soul);
        } else if (ChoiceName == "Open a Cell") {
            cout << "A prisoner made of prophecy steps out." << endl;
            encounterStarlessOracle(playerIP, musicManager);
        } else if (ChoiceName == "Pray to the Furnace") {
            cout << "The furnace blesses practical things: heat, iron, endurance." << endl;
            playerIP.hardiness += 1;
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Steal the Coals") {
            cout << "You steal living coals in your bare hands. Painful. Useful." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
            playerIP.permanentDamageModifier += 2;
        } else if (ChoiceName == "Repair the Altar") {
            if (playerIP.currentScrapMetal >= 2) {
                playerIP.currentScrapMetal -= 2;
                cout << "The repaired altar exhales approval through its vents." << endl;
                playerIP.maxHealth += 8;
                playerIP.heal(16);
            } else {
                cout << "You need 2 Scrap Metal to repair the altar." << endl;
            }
        } else if (ChoiceName == "Wake the Knight") {
            cout << "The Reforged Knight rises from prayer with a sword still orange from worship." << endl;
            encounterReforgedKnight(playerIP, musicManager);
        } else if (ChoiceName == "Walk Beneath Them") {
            cout << "You walk below the gargoyles like you do not know fear. They hate that." << endl;
            if (randomNum(1, 10) <= 6) {
                encounterCathedralGargoyle(playerIP, musicManager);
            } else {
                playerIP.permanentMomentum += 1;
            }
        } else if (ChoiceName == "Throw a Stone") {
            cout << "One gargoyle catches the stone in its mouth, chews, and drops from the roof." << endl;
            encounterCathedralGargoyle(playerIP, musicManager);
        } else if (ChoiceName == "Compliment the Stonework") {
            cout << "The gargoyles preen. Vanity is apparently not limited to living things." << endl;
            playerIP.heal(10);
        } else if (ChoiceName == "Climb to the Roof") {
            cout << "From the roof, you can see the Throne tower. It is not the tallest point, just the one everything else leans away from." << endl;
            playerIP.printMap();
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Pull the Trembling Blade") {
            cout << "The blade chooses to come free. That is different from permission." << endl;
            playerIP.permanentDamageModifier += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 6);
        } else if (ChoiceName == "Take Armor Plates") {
            cout << "You strap old royal plates under your gear. They are heavy with law." << endl;
            playerIP.hardiness += 2;
        } else if (ChoiceName == "Melt Scrap Into Edge") {
            if (playerIP.currentScrapMetal >= 2) {
                playerIP.currentScrapMetal -= 2;
                cout << "You ruin two pieces of scrap and improve one weapon. A fair exchange." << endl;
                playerIP.permanentDamageModifier += 2;
            } else {
                cout << "You need 2 Scrap Metal to do this." << endl;
            }
        } else if (ChoiceName == "Leave the Weapons") {
            cout << "You leave the room armed with restraint. Not impressive, but rare." << endl;
            playerIP.gainSoul(2);
        }
    }

public:
    explicit Citadel(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = Citadel::getChoice();
        Citadel::doEvent();
    }
};

class Beach : public BiomeEventDatabase {
private:
    vector<int> eventChances = {24, 18, 16, 14, 12, 10, 6};
    vector<string> eventNames = {"Tide Pools", "Shipwreck Ribcage", "Pearl Graves", "Moonlit Surf", "Salt Shrine", "Glass Crab March", "Old Lighthouse"};
    vector<string> eventFlavorText = {
        "The tide pools hold small trapped worlds. In one, a tiny storm rages over a tiny sea.",
        "A shipwreck lies cracked open like a whale skeleton. Its mast points inland, accusing the mountains.",
        "Pearls are arranged in rows beneath driftwood markers. Someone buried their dead with the ocean's teeth.",
        "The surf glows under moonlight even though it is day. Waves arrive out of order, some younger than others.",
        "A shrine of salt blocks stands where the waves cannot quite reach. Offerings dissolve before your eyes.",
        "Glass-shelled crabs march in a military line, each carrying a stolen button, coin, or bone chip.",
        "A lighthouse leans over the shore, its lamp still turning. It illuminates things the sun politely ignores."
    };
    vector<vector<string>> eventChoices = {
        {"Reach Into a Pool", "Study the Tiny Storm", "Drink the Pool", "Step Around Them"},
        {"Search the Captain's Room", "Pull the Mast", "Read the Logbook", "Sleep Below Deck"},
        {"Take a Pearl", "Count the Graves", "Leave a Soul", "Dig Carefully"},
        {"Walk Into the Surf", "Wait for the Wrong Wave", "Let it Wash Your Weapon", "Run From the Foam"},
        {"Offer Salt", "Lick the Shrine", "Break a Salt Block", "Pray to the Tide"},
        {"Follow the March", "Steal a Button", "Challenge the Biggest Crab", "Let Them Pass"},
        {"Climb the Lighthouse", "Turn the Lamp", "Extinguish the Lamp", "Sleep in the Keeper's Room"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Reach Into a Pool") {
            const int n = randomNum(1, 10);
            if (n <= 5) {
                cout << "You pull out a shell full of clean water and luck." << endl;
                playerIP.heal(12);
                playerIP.gainSoul(1);
            } else {
                cout << "Something in the pool was using the reflection as bait." << endl;
                encounterSlime(playerIP, musicManager);
            }
        } else if (ChoiceName == "Study the Tiny Storm") {
            cout << "The tiny storm teaches you that even small things can move with purpose." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Drink the Pool") {
            cout << "Salt water is a bad drink and an honest teacher." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 4);
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Step Around Them") {
            cout << "You avoid disturbing the little trapped worlds. Somewhere, a tiny civilization survives the day." << endl;
            playerIP.heal(5);
        } else if (ChoiceName == "Search the Captain's Room") {
            cout << "The captain kept maps, guilt, and a hidden piece of Scrap Metal." << endl;
            playerIP.currentScrapMetal += 1;
            if (randomNum(1, 10) <= 4) {
                encounterSkeleton(playerIP, musicManager);
            }
        } else if (ChoiceName == "Pull the Mast") {
            cout << "The mast falls and cracks open a sealed cargo hold." << endl;
            playerIP.currentScrapMetal += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 5);
        } else if (ChoiceName == "Read the Logbook") {
            cout << "The final entry says: The reef sang. We answered. The ship obeyed neither of us." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Sleep Below Deck") {
            cout << "The ship rocks though it is dead on sand. You sleep anyway." << endl;
            playerIP.heal(18);
        } else if (ChoiceName == "Take a Pearl") {
            cout << "The grave-pearl opens into an eye. A Lantern Bearer arrives to ask what you think you are worth." << endl;
            encounterLanternBearer(playerIP, musicManager);
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Count the Graves") {
            cout << "There are more graves than pearls, and more pearls than names." << endl;
            playerIP.gainSoul(3);
        } else if (ChoiceName == "Leave a Soul") {
            if (playerIP.soul >= 1) {
                playerIP.soul -= 1;
                cout << "The graves accept the Soul and give you a hush strong enough to hide inside." << endl;
                playerIP.hardiness += 1;
            } else {
                cout << "You cannot give what you do not have." << endl;
            }
        } else if (ChoiceName == "Dig Carefully") {
            cout << "You find a rusted charm, then the hand still wearing it." << endl;
            encounterSkeleton(playerIP, musicManager);
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Walk Into the Surf") {
            cout << "The wrong moon pulls the water around you like a cloak." << endl;
            playerIP.heal(8);
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Wait for the Wrong Wave") {
            cout << "The wrong wave arrives carrying something from tomorrow: pain, but useful pain." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 9);
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Let it Wash Your Weapon") {
            cout << "Salt and moonlight polish your blade until it remembers being ore." << endl;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Run From the Foam") {
            cout << "The foam chases like an animal. You learn speed honestly." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Offer Salt") {
            cout << "The shrine laughs silently. Salt given to salt is still devotion, apparently." << endl;
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Lick the Shrine") {
            cout << "Bad idea. Memorable, though." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 3);
            playerIP.heal(8);
        } else if (ChoiceName == "Break a Salt Block") {
            cout << "A shape of brine and hunger rises from inside the block." << endl;
            encounterMireLeech(playerIP, musicManager);
        } else if (ChoiceName == "Pray to the Tide") {
            cout << "The tide answers by moving one step closer, then one step away. Consent, maybe." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Follow the March") {
            cout << "The crabs lead you to a place where the beach has been sorting treasure for decades." << endl;
            playerIP.currentScrapMetal += randomNum(1, 3);
        } else if (ChoiceName == "Steal a Button") {
            cout << "The whole crab army turns at once." << endl;
            encounterIronScarab(playerIP, musicManager);
        } else if (ChoiceName == "Challenge the Biggest Crab") {
            cout << "The biggest crab is not a crab. It is an Iron Scarab wearing coastal confidence." << endl;
            encounterIronScarab(playerIP, musicManager);
            playerIP.hardiness += 1;
        } else if (ChoiceName == "Let Them Pass") {
            cout << "The crabs respect your restraint and leave behind one shiny tribute." << endl;
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Climb the Lighthouse") {
            cout << "From the top you see the Coral Reef glowing offshore and the Citadel inland like a bad thought." << endl;
            playerIP.printMap();
        } else if (ChoiceName == "Turn the Lamp") {
            cout << "The lamp reveals a hidden path through future water." << endl;
            playerIP.permanentMomentum += 1;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Extinguish the Lamp") {
            cout << "Darkness takes that personally." << endl;
            encounterStarlessOracle(playerIP, musicManager);
        } else if (ChoiceName == "Sleep in the Keeper's Room") {
            cout << "The keeper is gone, but the bed still knows how to forgive a traveler." << endl;
            playerIP.heal(20);
        }
    }

public:
    explicit Beach(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = Beach::getChoice();
        Beach::doEvent();
    }
};

class Mountains : public BiomeEventDatabase {
private:
    vector<int> eventChances = {23, 18, 16, 14, 12, 10, 7};
    vector<string> eventNames = {"Knife Pass", "Avalanche Bowl", "Hermit's Smoke", "Sky Altar", "Dragon Bones", "Frozen Watchtower", "Thin Air"};
    vector<string> eventFlavorText = {
        "The path narrows until the mountain seems to be deciding which side of you to keep.",
        "Snow fills a wide stone bowl. Every sound you make comes back larger and more dangerous.",
        "A thread of smoke rises from a hut wedged between impossible rocks. Someone lives here by argument with gravity.",
        "An altar of blue stone stands above the clouds. Birds avoid flying over it.",
        "A dragon skeleton coils around the ridge. Its skull is large enough to hold a chapel inside the jaw.",
        "A watchtower stands frozen shut. The guards inside are still at their posts, or something shaped like guards is.",
        "The air grows thin enough that each breath feels borrowed. The mountain waits to see what you will spend it on."
    };
    vector<vector<string>> eventChoices = {
        {"Edge Along", "Jump the Gap", "Kick Loose Stones", "Turn Around"},
        {"Whisper", "Shout", "Cross Fast", "Dig for Supplies"},
        {"Enter the Hut", "Trade Scrap", "Ask About Dragons", "Steal from the Porch"},
        {"Pray Above the Clouds", "Offer a Soul", "Deface the Altar", "Watch the Birds"},
        {"Enter the Skull", "Take a Tooth", "Listen to the Bones", "Sleep in the Ribcage"},
        {"Open the Door", "Climb the Outside", "Signal with Fire", "Salute the Guards"},
        {"Keep Climbing", "Rest and Breathe", "Laugh at the Height", "Bleed into the Snow"}
    };

    string ChoiceName;
    int REI = -1;

protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];
        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Edge Along") {
            cout << "You press yourself against the mountain and become temporarily religious about friction." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Jump the Gap") {
            const int n = randomNum(1, 10);
            if (n <= 6) {
                cout << "You clear the gap. Your fear arrives slightly after you do." << endl;
                playerIP.permanentMomentum += 2;
            } else {
                cout << "You do not clear it cleanly. The ledge catches you, but not gently." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, randomNum(12, 22));
            }
        } else if (ChoiceName == "Kick Loose Stones") {
            cout << "The stones fall. Something below howls up at you." << endl;
            encounterAshenHound(playerIP, musicManager);
        } else if (ChoiceName == "Turn Around") {
            cout << "You turn around before the pass can make a decision. Pride is lighter than a corpse." << endl;
            playerIP.heal(4);
        } else if (ChoiceName == "Whisper") {
            cout << "The avalanche sleeps through your whisper. You cross in the kind of silence that becomes skill." << endl;
            playerIP.permanentMomentum += 1;
            playerIP.hardiness += 1;
        } else if (ChoiceName == "Shout") {
            cout << "The mountain answers with everything loose." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, randomNum(18, 28));
            if (randomNum(1, 10) <= 5) {
                encounterGlassSpider(playerIP, musicManager);
            }
        } else if (ChoiceName == "Cross Fast") {
            cout << "You outrun the first slide and are politely clipped by the second." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 7);
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Dig for Supplies") {
            cout << "You find a frozen pack containing Scrap Metal and something that used to be lunch." << endl;
            playerIP.currentScrapMetal += 1;
            playerIP.heal(5);
        } else if (ChoiceName == "Enter the Hut") {
            cout << "The hermit is gone, but their chair rocks anyway. A note says: Do not trust crowns or eggs." << endl;
            playerIP.maxSoul += 1;
            playerIP.gainSoul(2);
        } else if (ChoiceName == "Trade Scrap") {
            if (playerIP.currentScrapMetal > 0) {
                playerIP.currentScrapMetal -= 1;
                cout << "The absent hermit accepts the trade. A packet of bitter herbs appears on the table." << endl;
                playerIP.maxHealth += 5;
                playerIP.heal(15);
            } else {
                cout << "You have no Scrap Metal. The empty hut still judges your bargaining posture." << endl;
            }
        } else if (ChoiceName == "Ask About Dragons") {
            cout << "The smoke curls into a dragon shape, then into a warning: They do not die. They become places." << endl;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Steal from the Porch") {
            cout << "A hound made of ash unfolds from the fireplace and follows your theft outside." << endl;
            encounterAshenHound(playerIP, musicManager);
            playerIP.currentScrapMetal += 2;
        } else if (ChoiceName == "Pray Above the Clouds") {
            cout << "There are fewer things between you and the gods up here. Unfortunately, that includes excuses." << endl;
            playerIP.maxSoul += 2;
            playerIP.heal(8);
        } else if (ChoiceName == "Offer a Soul") {
            if (playerIP.soul >= 1) {
                playerIP.soul -= 1;
                cout << "The altar takes the Soul and gives you altitude in your blood." << endl;
                playerIP.permanentMomentum += 1;
                playerIP.hardiness += 1;
            } else {
                cout << "The altar receives your empty gesture and returns empty wind." << endl;
            }
        } else if (ChoiceName == "Deface the Altar") {
            cout << "The sky sends a Cathedral Gargoyle, which seems unfair but thematically consistent." << endl;
            encounterCathedralGargoyle(playerIP, musicManager);
        } else if (ChoiceName == "Watch the Birds") {
            cout << "The birds know where the updrafts are. By watching them, so do you." << endl;
            playerIP.permanentMomentum += 1;
        } else if (ChoiceName == "Enter the Skull") {
            cout << "Inside the skull, old heat still clings to the bone." << endl;
            if (randomNum(1, 10) <= 4) {
                encounterCinderDragon(playerIP, musicManager);
            } else {
                playerIP.maxSoul += 2;
            }
        } else if (ChoiceName == "Take a Tooth") {
            cout << "The tooth is heavier than a sword and sharper than advice." << endl;
            playerIP.permanentDamageModifier += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 6);
        } else if (ChoiceName == "Listen to the Bones") {
            cout << "The bones remember flying. For a moment, so do your feet." << endl;
            playerIP.permanentMomentum += 2;
        } else if (ChoiceName == "Sleep in the Ribcage") {
            cout << "You sleep where a dragon's heart used to argue with the world." << endl;
            playerIP.heal(25);
            playerIP.gainSoul(3);
        } else if (ChoiceName == "Open the Door") {
            cout << "The frozen guards turn their heads one crack at a time." << endl;
            encounterSkeleton(playerIP, musicManager);
            encounterReforgedKnight(playerIP, musicManager);
        } else if (ChoiceName == "Climb the Outside") {
            cout << "You climb the icy stones and enter through a window like a disrespectful ghost." << endl;
            playerIP.permanentMomentum += 1;
            playerIP.currentScrapMetal += 1;
        } else if (ChoiceName == "Signal with Fire") {
            cout << "Something far higher answers your signal with a coal-red blink." << endl;
            playerIP.maxSoul += 1;
            if (randomNum(1, 10) <= 3) {
                encounterCinderDragon(playerIP, musicManager);
            }
        } else if (ChoiceName == "Salute the Guards") {
            cout << "The frozen guards salute back. Your spine becomes briefly straighter." << endl;
            playerIP.hardiness += 1;
        } else if (ChoiceName == "Keep Climbing") {
            cout << "Every step is an argument with your lungs. You win just enough." << endl;
            playerIP.permanentMomentum += 1;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 5);
        } else if (ChoiceName == "Rest and Breathe") {
            cout << "You sit, breathe, and let the mountain fail to hurry you." << endl;
            playerIP.heal(14);
        } else if (ChoiceName == "Laugh at the Height") {
            cout << "The height does not understand the joke, but your courage does." << endl;
            playerIP.maxSoul += 1;
            playerIP.permanentDamageModifier += 1;
        } else if (ChoiceName == "Bleed into the Snow") {
            cout << "Your blood steams black-red on white. The mountain accepts the signature." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
            playerIP.hardiness += 2;
        }
    }

public:
    explicit Mountains(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = Mountains::getChoice();
        Mountains::doEvent();
    }
};

class SandPit : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Sand Pit"};
    vector<string> eventFlavorText = {"The sand here pours downward forever, not into a hole, but into a decision. Every grain whispers the same word: descend."};
    vector<vector<string>> eventChoices = {{"Descend", "Throw Scrap First", "Meditate at the Edge", "Leave"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Descend") {
            cout << "You slide into the pit and learn that falling can be steered if you stop arguing with gravity." << endl;
            if (!ArmagestaBiomeTools::hasCombatAction(playerIP, "Sandstep")) {
                encounterDuneMaw(playerIP, musicManager);
                if (playerIP.health > 0) {
                    ArmagestaBiomeTools::unlockCombatAction(playerIP, "Sandstep", "Spend 2 Souls to dodge with desert footwork, gain Momentum, and raise defense for the turn.");
                }
            } else {
                cout << "The pit recognizes your footwork and lets you ride the sand safely." << endl;
                playerIP.heal(12);
            }
        } else if (ChoiceName == "Throw Scrap First") {
            if (playerIP.currentScrapMetal > 0) {
                playerIP.currentScrapMetal -= 1;
                cout << "The scrap vanishes. The pit shows you where it went by drawing a path in sinking sand." << endl;
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Sandstep", "Spend 2 Souls to dodge with desert footwork, gain Momentum, and raise defense for the turn.");
            } else {
                cout << "You have no Scrap Metal. The pit waits, unimpressed." << endl;
            }
        } else if (ChoiceName == "Meditate at the Edge") {
            cout << "You watch the sand fall until your own panic seems slow and avoidable." << endl;
            playerIP.permanentMomentum += 1;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Leave") {
            cout << "You leave the pit for now. It continues falling without you." << endl;
        }
    }
public:
    explicit SandPit(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = SandPit::getChoice();
        SandPit::doEvent();
    }
};

class CrystalGeode : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Crystal Geode"};
    vector<string> eventFlavorText = {"The cave opens into a perfect crystal heart. Light grows here like moss, and every shard reflects a version of you that survived something different."};
    vector<vector<string>> eventChoices = {{"Touch the Heart Crystal", "Break a Reflection", "Kneel in the Light", "Leave Carefully"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Touch the Heart Crystal") {
            cout << "The heart crystal grows around your hand, then decides you are not worth keeping." << endl;
            if (!ArmagestaBiomeTools::hasCombatAction(playerIP, "Crystal Ward")) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Crystal Ward", "Spend 3 Souls to create a crystal defense wall. If an enemy attacks, broken light damages it back.");
                playerIP.hardiness += 1;
            } else {
                playerIP.heal(15);
            }
        } else if (ChoiceName == "Break a Reflection") {
            cout << "The broken reflection crawls out as the Crystal Matriarch's anger." << endl;
            encounterCrystalMatriarch(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.currentScrapMetal += 3;
            }
        } else if (ChoiceName == "Kneel in the Light") {
            cout << "The geode shines through you and finds the cracks. Instead of widening them, it lines them with color." << endl;
            playerIP.maxSoul += 2;
            playerIP.heal(20);
        } else if (ChoiceName == "Leave Carefully") {
            cout << "You leave without touching anything. The reflections seem disappointed, but respectful." << endl;
        }
    }
public:
    explicit CrystalGeode(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = CrystalGeode::getChoice();
        CrystalGeode::doEvent();
    }
};

class OldestRedwood : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Oldest Redwood"};
    vector<string> eventFlavorText = {"This redwood is not a tree so much as a vertical country. Its bark contains closed doors, old faces, and one heartbeat too slow for human fear."};
    vector<vector<string>> eventChoices = {{"Speak the Old Oath", "Climb Inside the Bark", "Challenge the Elder", "Sleep at its Roots"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Speak the Old Oath") {
            cout << "You do not know the words until the tree says them through you." << endl;
            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Redwood Oath", "Spend 5 Souls to heal, gain defense, empower your attacks, and weaken the enemy with ancient roots.");
            playerIP.maxHealth += 5;
        } else if (ChoiceName == "Climb Inside the Bark") {
            cout << "Inside the tree are rooms grown for people who never arrived." << endl;
            playerIP.maxSoul += 2;
            playerIP.printMap();
        } else if (ChoiceName == "Challenge the Elder") {
            cout << "The Oldest Redwood does not fight you itself. It sends an Ancient Ent like a raised eyebrow." << endl;
            encounterAncientEnt(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.hardiness += 3;
            }
        } else if (ChoiceName == "Sleep at its Roots") {
            cout << "You sleep for one night and dream for one century." << endl;
            playerIP.heal(30);
            playerIP.gainSoul(5);
        }
    }
public:
    explicit OldestRedwood(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = OldestRedwood::getChoice();
        OldestRedwood::doEvent();
    }
};

class WitchsHut : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Witch's Hut"};
    vector<string> eventFlavorText = {"The hut stands on stilts made of bird bones and bad decisions. Smoke leaks from the chimney in the shape of question marks."};
    vector<vector<string>> eventChoices = {{"Knock Three Times", "Drink from the Cauldron", "Buy a Hex", "Steal the Broom"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Knock Three Times") {
            cout << "The witch opens the door before the third knock and charges you for the unused one." << endl;
            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Bogblood Hex", "Pay 6 Health to deal hex damage, heavily weaken the enemy, and regain 1 Soul.");
        } else if (ChoiceName == "Drink from the Cauldron") {
            cout << "The cauldron tastes like mud, lightning, and someone else's memory." << endl;
            playerIP.maxSoul += 2;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
        } else if (ChoiceName == "Buy a Hex") {
            if (playerIP.currentScrapMetal >= 2) {
                playerIP.currentScrapMetal -= 2;
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Bogblood Hex", "Pay 6 Health to deal hex damage, heavily weaken the enemy, and regain 1 Soul.");
                playerIP.permanentDamageModifier += 1;
            } else {
                cout << "The witch wants 2 Scrap Metal. She suggests you come back when you are more interesting." << endl;
            }
        } else if (ChoiceName == "Steal the Broom") {
            cout << "The broom screams for its sisters. The Marionette Coven answers instead." << endl;
            encounterMarionetteCoven(playerIP, musicManager);
        }
    }
public:
    explicit WitchsHut(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = WitchsHut::getChoice();
        WitchsHut::doEvent();
    }
};

class TheThrone : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"The Throne"};
    vector<string> eventFlavorText = {"The throne room has no roof. The sky itself looks down like a witness. The throne waits, empty and offended."};
    vector<vector<string>> eventChoices = {{"Sit on the Throne", "Break the Crown", "Read the Law", "Bow and Leave"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Sit on the Throne") {
            cout << "The throne tries to decide whether you are a ruler, a thief, or fuel." << endl;
            encounterStarlessOracle(playerIP, musicManager);
            if (playerIP.health > 0) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Royal Decree", "Spend 3 Souls and 2 Momentum to forbid an enemy attack from landing, or condemn a passive enemy.");
                playerIP.maxSoul += 1;
            }
        } else if (ChoiceName == "Break the Crown") {
            cout << "The crown breaks like a chain. The Citadel shakes with relief and rage." << endl;
            encounterReforgedKnight(playerIP, musicManager);
            playerIP.permanentDamageModifier += 2;
        } else if (ChoiceName == "Read the Law") {
            cout << "The first law is survival. The second is obedience. The third has been scratched out by many hands." << endl;
            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Royal Decree", "Spend 3 Souls and 2 Momentum to forbid an enemy attack from landing, or condemn a passive enemy.");
        } else if (ChoiceName == "Bow and Leave") {
            cout << "The empty throne accepts respect better than occupation." << endl;
            playerIP.heal(20);
            playerIP.gainSoul(3);
        }
    }
public:
    explicit TheThrone(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = TheThrone::getChoice();
        TheThrone::doEvent();
    }
};

class CoralReef : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Coral Reef"};
    vector<string> eventFlavorText = {"The reef glows beneath clear water like a city that chose to become an animal. Currents twist around it in careful, deliberate sentences."};
    vector<vector<string>> eventChoices = {{"Dive to the Heart", "Take a Coral Crown", "Listen to the Current", "Surface"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Dive to the Heart") {
            cout << "At the reef's heart, the Tide Leviathan coils around an altar of pearls." << endl;
            encounterTideLeviathan(playerIP, musicManager);
            if (playerIP.health > 0) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Tidecall", "Spend 4 Souls to call a defensive tide that damages the enemy and weakens its Danger.");
            }
        } else if (ChoiceName == "Take a Coral Crown") {
            cout << "The crown cuts your hands and crowns your blood." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 8);
            playerIP.hardiness += 2;
            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Tidecall", "Spend 4 Souls to call a defensive tide that damages the enemy and weakens its Danger.");
        } else if (ChoiceName == "Listen to the Current") {
            cout << "The current teaches you how to move before impact instead of after pain." << endl;
            playerIP.permanentMomentum += 2;
            playerIP.maxSoul += 1;
        } else if (ChoiceName == "Surface") {
            cout << "You surface with salt in your eyes and a strange mercy in your lungs." << endl;
            playerIP.heal(16);
        }
    }
public:
    explicit CoralReef(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = CoralReef::getChoice();
        CoralReef::doEvent();
    }
};

class DragonsLair : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Dragon's Lair"};
    vector<string> eventFlavorText = {"The lair is not inside the mountain. The mountain is inside the lair. Gold, bones, and old fire make a nest large enough for the end of a story."};
    vector<vector<string>> eventChoices = {{"Challenge the Dragon", "Steal an Ember", "Offer the Hoard", "Retreat"}};
    string ChoiceName;
    int REI = -1;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void ResolveChoice() override {
        if (ChoiceName == "Challenge the Dragon") {
            cout << "The Cinder Dragon opens one eye. It has been awake for years, waiting for someone rude enough to call this a beginning." << endl;
            encounterCinderDragon(playerIP, musicManager);
            if (playerIP.health > 0) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Dragonfire", "Spend 8 Souls to breathe stolen dragonfire for massive damage, burning yourself in the process.");
                playerIP.maxSoul += 2;
                playerIP.permanentDamageModifier += 2;
            }
        } else if (ChoiceName == "Steal an Ember") {
            cout << "The ember burns through the idea of your hand before reaching the skin." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 15);
            if (randomNum(1, 10) <= 5) {
                encounterCinderDragon(playerIP, musicManager);
            }
            if (playerIP.health > 0) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Dragonfire", "Spend 8 Souls to breathe stolen dragonfire for massive damage, burning yourself in the process.");
            }
        } else if (ChoiceName == "Offer the Hoard") {
            if (playerIP.currentScrapMetal >= 5) {
                playerIP.currentScrapMetal -= 5;
                cout << "The dragon accepts your metal, not as tribute, but as an apology from the age of swords." << endl;
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Dragonfire", "Spend 8 Souls to breathe stolen dragonfire for massive damage, burning yourself in the process.");
                playerIP.maxHealth += 10;
            } else {
                cout << "The dragon snorts. Bring 5 Scrap Metal if you intend to insult it economically." << endl;
            }
        } else if (ChoiceName == "Retreat") {
            cout << "The dragon lets you leave. Mercy, from something that large, feels exactly like being spared by weather." << endl;
            playerIP.heal(10);
        }
    }
public:
    explicit DragonsLair(Player& PlayerIP, BackgroundMusicManager& musicManager) : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = DragonsLair::getChoice();
        DragonsLair::doEvent();
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
