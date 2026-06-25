//
// Created by Taylor Anderson on 6/16/26.
//

#ifndef ARMAGESTA_EVENTDATABASE_H
#define ARMAGESTA_EVENTDATABASE_H
#include <iostream>
#include <string>
#include <cctype>
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




namespace ArmagestaBiomeTools {
    inline string lowered(string value) {
        for (char& letter : value) {
            letter = static_cast<char>(tolower(static_cast<unsigned char>(letter)));
        }
        return value;
    }

    inline bool containsText(const string& text, const string& wantedText) {
        return lowered(text).find(lowered(wantedText)) != string::npos;
    }

    inline string getChoiceMechanicalPreview(const string& choiceName) {
        const string choice = lowered(choiceName);

        // These are intentionally suggestive instead of exact. They let the player understand
        // the likely kind of consequence without spoiling every random result.
        if (choice == "rest" || containsText(choice, "sleep") || containsText(choice, "breathe")) {
            return "Recovery path: likely restores Health, Souls, or gives a small safe benefit.";
        }

        if (choice == "move on" || choice == "turn back" || choice == "retreat" || choice == "leave" ||
            choice == "leave carefully" || choice == "leave quietly" || choice == "leave it alone" ||
            choice == "step away" || choice == "surface" || choice == "back away slowly" ||
            choice == "bow and leave") {
            return "Cautious path: usually avoids the main danger, often with a smaller reward.";
        }

        if (containsText(choice, "scrap") || containsText(choice, "pay") || containsText(choice, "offer") ||
            containsText(choice, "pour souls") || containsText(choice, "leave a soul") ||
            containsText(choice, "feed it souls") || containsText(choice, "buy")) {
            return "Resource trade: may spend Scrap Metal or Souls to avoid danger or gain a lasting benefit.";
        }

        if (containsText(choice, "fight") || containsText(choice, "challenge") || containsText(choice, "confront") ||
            containsText(choice, "wake") || containsText(choice, "call into") || containsText(choice, "call out") ||
            containsText(choice, "shout") || containsText(choice, "open a cell") ||
            containsText(choice, "challenge the dragon")) {
            return "Danger path: likely starts combat, but can lead to stronger rewards or unlocks.";
        }

        if (containsText(choice, "steal") || containsText(choice, "rob") || containsText(choice, "loot") ||
            containsText(choice, "break") || containsText(choice, "shatter") || containsText(choice, "cut") ||
            containsText(choice, "kick") || containsText(choice, "deface") || containsText(choice, "stab") ||
            containsText(choice, "pull") || containsText(choice, "throw")) {
            return "Aggressive path: higher risk of damage or combat, usually with a stronger material reward.";
        }

        if (containsText(choice, "search") || containsText(choice, "scavenge") || containsText(choice, "dig") ||
            containsText(choice, "collect") || containsText(choice, "harvest") || containsText(choice, "take") ||
            containsText(choice, "reach")) {
            return "Resource path: likely searches for loot, Scrap Metal, healing, or a useful upgrade.";
        }

        if (containsText(choice, "read") || containsText(choice, "study") || containsText(choice, "listen") ||
            containsText(choice, "ask") || containsText(choice, "count") || containsText(choice, "mark") ||
            containsText(choice, "watch") || containsText(choice, "hear")) {
            return "Lore path: likely reveals story, objectives, or a careful stat/soul benefit.";
        }

        if (containsText(choice, "pray") || containsText(choice, "kneel") || containsText(choice, "bow") ||
            containsText(choice, "confess") || containsText(choice, "salute") || containsText(choice, "speak") ||
            containsText(choice, "answer") || containsText(choice, "argue") || containsText(choice, "refuse")) {
            return "Spirit path: likely affects Souls, quests, judgment, or a permanent character stat.";
        }

        if (containsText(choice, "drink") || containsText(choice, "taste") || containsText(choice, "lick") ||
            containsText(choice, "blood") || containsText(choice, "bleed")) {
            return "Body-risk path: may heal or empower you, but can also cost Health.";
        }

        if (containsText(choice, "follow") || containsText(choice, "travel") || containsText(choice, "enter") ||
            containsText(choice, "ride") || containsText(choice, "climb") || containsText(choice, "cross") ||
            containsText(choice, "walk") || containsText(choice, "step") || containsText(choice, "jump") ||
            containsText(choice, "sprint") || containsText(choice, "swim") || containsText(choice, "crawl") ||
            containsText(choice, "edge along") || containsText(choice, "dive") || containsText(choice, "descend")) {
            return "Movement path: likely tests risk versus progress, often affecting Momentum or Health.";
        }

        if (containsText(choice, "light") || containsText(choice, "extinguish") || containsText(choice, "ring") ||
            containsText(choice, "sing") || containsText(choice, "hum") || containsText(choice, "signal")) {
            return "Signal path: may summon help, danger, lore, or a special reward.";
        }

        return "Uncertain path: likely changes Health, Souls, resources, quests, or combat risk.";
    }

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
            cout << "Only path:" << endl;
            cout << "1: " << choices[0] << endl;
            cout << "     " << getChoiceMechanicalPreview(choices[0]) << endl << endl;
            return 0;
        }

        cout << "Choose one:" << endl;
        for (int i = 0; i < choices.size(); i++) {
            cout << i + 1 << ": " << choices[i] << endl;
            cout << "     " << getChoiceMechanicalPreview(choices[i]) << endl;
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
        cout << eventFlavorText << endl;
        cout << endl;
    }
}

namespace ArmagestaNonCombatEvents {
    inline void desertWaystone(Player& playerIP) {
        playerIP.addStoryFlag("Desert Waystone");
        cout << endl << "===== Desert Waystone =====" << endl;
        cout << "A sunken waystone rises from the sand, its top just high enough to cast a shadow in every direction at once. The map carved across it is made of darkness instead of lines, and the darkness shifts whenever you look away. Someone wanted travelers to find the Sand Pit, but not all travelers to survive finding it." << endl;
        vector<string> choices = {"Read the Shadow Map", "Press Your Palm to the Stone", "Leave It Alone"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Read the Shadow Map") {
            cout << "The shadows show a pit that falls beneath the desert. The name Sand Pit burns into your memory." << endl;
            playerIP.addQuest("Search the Desert for the Sand Pit");
            playerIP.maxSoul += 1;
        } else if (choices[choice] == "Press Your Palm to the Stone") {
            cout << "The stone drinks your heat and gives back discipline. Your steps feel lighter." << endl;
            playerIP.permanentMomentum += 1;
            playerIP.health -= 4;
            makeZeroIfNegative(playerIP.health);
        } else {
            cout << "You leave the waystone unread. The desert does not seem offended. It has time." << endl;
        }
    }

    inline void caveResonance(Player& playerIP) {
        playerIP.addStoryFlag("Cave Resonance Door");
        cout << endl << "===== Resonance Door =====" << endl;
        cout << "A sealed stone door hums when you breathe, answering your lungs before you have chosen to speak. There is no keyhole, no handle, and no visible hinge, only stone worn smooth by generations of desperate palms. It is not locked by metal; it is locked by memory, and yours seems close enough to fit." << endl;
        vector<string> choices = {"Hum With It", "Carve a Mark", "Step Away"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Hum With It") {
            cout << "Your voice becomes three voices, then one. The door never opens, but your soul does." << endl;
            playerIP.maxSoul += 2;
            playerIP.gainSoul(2);
            playerIP.addQuest("Find the Crystal Geode beneath the Caves");
        } else if (choices[choice] == "Carve a Mark") {
            cout << "Your mark fills with mineral light. You chip away a useful shard of metal-rich crystal." << endl;
            playerIP.currentScrapMetal += 2;
            playerIP.health -= 5;
            makeZeroIfNegative(playerIP.health);
        } else {
            cout << "The door continues humming after you leave. It has learned the shape of your footsteps." << endl;
        }
    }

    inline void redwoodMemory(Player& playerIP) {
        playerIP.addStoryFlag("Redwood Memory Knot");
        cout << endl << "===== Memory Knot =====" << endl;
        cout << "A redwood root has grown around an old helmet, holding it as carefully as a hand around a candle. The metal is rusted, but the visor still points toward the Citadel as if the dead soldier inside never stopped watching. The wood has not crushed the relic; it has preserved it, and preservation feels more judgmental than kindness here." << endl;
        vector<string> choices = {"Listen to the Helmet", "Pull It Free", "Bury It Deeper"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Listen to the Helmet") {
            cout << "A dead soldier whispers that the Oldest Redwood remembers the first throne and the first betrayal." << endl;
            playerIP.addQuest("Seek the Oldest Redwood");
            playerIP.permanentMomentum += 1;
        } else if (choices[choice] == "Pull It Free") {
            cout << "The helmet breaks, but its metal braces your arm." << endl;
            playerIP.hardiness += 1;
            playerIP.currentScrapMetal += 1;
        } else {
            cout << "You bury it deeper. The forest approves of things being allowed to finish dying." << endl;
            playerIP.heal(10);
        }
    }

    inline void swampFerry(Player& playerIP) {
        playerIP.addStoryFlag("Swamp Ferry Contract");
        cout << endl << "===== Empty Ferry =====" << endl;
        cout << "A ferry bumps against the bank with no ferryman, knocking three times against the mud as if asking to be invited ashore. A contract lies on the seat, already signed in pondwater and sealed with a thumbprint that may not be human. The fog behind the boat parts just enough to suggest that the river knows your destination better than you do." << endl;
        vector<string> choices = {"Pay One Soul", "Sign in Blood", "Shove the Ferry Away"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Pay One Soul") {
            if (playerIP.soul >= 1) {
                playerIP.soul -= 1;
                cout << "The ferry carries you three feet into the fog and back. Somehow, you return older and sturdier." << endl;
                playerIP.maxHealth += 5;
                playerIP.addQuest("Find the Witch's Hut in the Swamp");
            } else {
                cout << "You have no soul to spare. The contract folds itself shut." << endl;
            }
        } else if (choices[choice] == "Sign in Blood") {
            cout << "The contract accepts red ink. Your blood feels like it learned a curse." << endl;
            playerIP.health -= 8;
            makeZeroIfNegative(playerIP.health);
            playerIP.permanentDamageModifier += 1;
        } else {
            cout << "The ferry drifts away, offended in the way only empty things can be." << endl;
            playerIP.gainSoul(1);
        }
    }

    inline void citadelArmory(Player& playerIP) {
        playerIP.addStoryFlag("Citadel Silent Armory");
        cout << endl << "===== Silent Armory =====" << endl;
        cout << "You find an armory where every weapon is chained to the wall, each blade displayed like a criminal awaiting sentence. The chains are ceremonial, not practical; any strong hand could break them, but every instinct tells you that would still count as treason. Dust lies over the room in the shape of soldiers who never returned for their steel." << endl;
        vector<string> choices = {"Sharpen Your Blade", "Study the Shields", "Read the Oath on the Wall"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Sharpen Your Blade") {
            cout << "You borrow the edge of a dead kingdom." << endl;
            playerIP.permanentDamageModifier += 1;
        } else if (choices[choice] == "Study the Shields") {
            cout << "The shield forms are brutal, simple, and useful." << endl;
            playerIP.hardiness += 2;
        } else {
            cout << "The oath says: A throne without a dragon is a chair. A dragon without a throne is a disaster." << endl;
            playerIP.addQuest("Claim The Throne after defeating the Cinder Dragon");
            playerIP.maxSoul += 1;
        }
    }

    inline void beachTideCalendar(Player& playerIP) {
        playerIP.addStoryFlag("Beach Tide Calendar");
        cout << endl << "===== Tide Calendar =====" << endl;
        cout << "Flat stones line the shore in a circle, each one carved with a moon phase, a wave mark, or a name too eroded to trust. The tide touches them in an order that looks intentional, pausing at some and avoiding others like a priest choosing who deserves blessing. Beneath the sound of surf, you can almost hear something counting with you from underwater." << endl;
        vector<string> choices = {"Count the Tide", "Sleep in the Circle", "Kick the Stones Apart"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Count the Tide") {
            cout << "The rhythm points seaward, toward a reef that glows under moonless water." << endl;
            playerIP.addQuest("Find the Coral Reef");
            playerIP.permanentMomentum += 1;
        } else if (choices[choice] == "Sleep in the Circle") {
            cout << "You dream of lungs full of salt and wake breathing easier." << endl;
            playerIP.heal(20);
            playerIP.maxSoul += 1;
        } else {
            cout << "The tide immediately rebuilds the circle. You feel judged by water." << endl;
            playerIP.health -= 6;
            makeZeroIfNegative(playerIP.health);
            playerIP.currentScrapMetal += 1;
        }
    }

    inline void mountainBell(Player& playerIP) {
        playerIP.addStoryFlag("Mountain Pilgrim Bell");
        cout << endl << "===== Pilgrim Bell =====" << endl;
        cout << "A bronze bell hangs from a frozen arch, green with age except where countless hands have polished it bright. Its clapper is a dragon tooth, worn smooth by pilgrims who came here to ask the mountain for courage, forgiveness, or death. The bell has not rung in years, but the snow beneath it is cracked in perfect circles." << endl;
        vector<string> choices = {"Ring It Once", "Pray Beneath It", "Break the Tooth Free"};
        int choice = ArmagestaBiomeTools::askChoice(choices);

        if (choices[choice] == "Ring It Once") {
            cout << "The sound climbs higher than the mountain. Far away, something enormous shifts in its sleep." << endl;
            playerIP.addQuest("Find Dragon's Lair");
            playerIP.permanentMomentum += 1;
        } else if (choices[choice] == "Pray Beneath It") {
            cout << "Your prayer freezes before leaving your mouth, then falls back into you as resolve." << endl;
            playerIP.maxHealth += 5;
            playerIP.gainSoul(2);
        } else {
            cout << "The tooth snaps loose. It is almost too hot to carry." << endl;
            playerIP.health -= 10;
            makeZeroIfNegative(playerIP.health);
            playerIP.permanentDamageModifier += 2;
        }
    }

    inline void maybeTrigger(Player& playerIP) {
        // Roughly one non-combat progression event every few moves.
        if (randomNum(1, 100) > 22) {
            return;
        }

        char biomeType = playerIP.getCurrentBiomeType();

        if (biomeType == 'D' && !playerIP.hasStoryFlag("Desert Waystone")) {
            desertWaystone(playerIP);
        } else if (biomeType == 'C' && !playerIP.hasStoryFlag("Cave Resonance Door")) {
            caveResonance(playerIP);
        } else if (biomeType == 'R' && !playerIP.hasStoryFlag("Redwood Memory Knot")) {
            redwoodMemory(playerIP);
        } else if (biomeType == 'W' && !playerIP.hasStoryFlag("Swamp Ferry Contract")) {
            swampFerry(playerIP);
        } else if (biomeType == 'I' && !playerIP.hasStoryFlag("Citadel Silent Armory")) {
            citadelArmory(playerIP);
        } else if (biomeType == 'B' && !playerIP.hasStoryFlag("Beach Tide Calendar")) {
            beachTideCalendar(playerIP);
        } else if (biomeType == 'M' && !playerIP.hasStoryFlag("Mountain Pilgrim Bell")) {
            mountainBell(playerIP);
        }
    }
}

class Forest : public BiomeEventDatabase {
private:
    vector<int> eventChances = {60, 40, 5, 5};

    vector<string> eventNames = {
        "Bright Forest",
        "Dark Brush",
        "Hidden Cathedral",
        "Grand Tree"
    };

    vector<string> eventFlavorText = {
        "Light pours through the canopy in broad green shafts, catching on drifting pollen and the pale backs of leaves. The forest floor glows as if the earth itself is remembering spring, and for one dangerous moment Armagesta almost seems gentle. Even here, though, the quiet has edges; birds stop singing whenever your hand nears your weapon.",
        "Whispers flow through the trees in overlapping layers, some high as insects and some low as buried men. Something soft-bodied drags itself through the brush nearby, pausing whenever you pause, breathing whenever you forget to. The darker leaves are wet with something that is not rain, and every path ahead seems to narrow toward the sound.",
        "A tall cathedral hidden in the trees stands before you, its roof split by branches and its saints softened under moss. Vines pass through broken windows like green fingers searching old wounds, and the bell tower leans against a redwood as if exhausted. The forest has not destroyed this holy place; it has claimed it, and the claim feels older than the religion that built the stones.",
        "A tree towers before you, at least three times taller than the giants around it, its crown vanishing into a private weather of leaves. Knots in the bark bend like an old face trying to remember your name, and the roots have pushed ancient bones gently aside instead of breaking them. Something about the tree suggests it has watched heroes climb, carve, pray, and fall, and it has learned to expect disappointment."
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
        return ArmagestaBiomeTools::weightedChoice(eventChances);
    }

    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);

        const int chosenChoice = ArmagestaBiomeTools::askChoice(eventChoices[REI]);
        ChoiceName = eventChoices[REI][chosenChoice];

        ResolveChoice();
    }

    void ResolveChoice() override {
        if (ChoiceName == "Rest") {
            playerIP.heal(2);
            cout << "You rest beneath the trees. The leaves shift above you like quiet hands." << endl;
            cout << "You healed 2 Health." << endl;

        } else if (ChoiceName == "Scavenge") {
            const int n = randomNum(1, 10);

            if (n <= 6) {
                cout << "You search under moss, roots, and fallen leaves, but find nothing useful." << endl;
            } else if (n <= 8) {
                cout << "You find a piece of Scrap Metal wedged inside an old stump." << endl;
                playerIP.currentScrapMetal += 1;
            } else if (n <= 9) {
                cout << "You find a juicy plum hanging from a branch that seems too low by choice." << endl;
                playerIP.heal(10);
                cout << "You healed 10 Health." << endl;
            } else {
                cout << "You find a Hearty Salmon wrapped in wet leaves beside a cold cooking stone." << endl;
                cout << "After some light cooking, it becomes quite the meal." << endl;
                playerIP.maxHealth += 5;
                playerIP.heal(5);
                cout << "Your Max Health increased by 5." << endl;
            }

        } else if (ChoiceName == "Confront") {
            cout << "You step toward the sound in the brush." << endl;
            cout << "A Slime appears!" << endl;
            encounterSlime(playerIP, musicManager);

        } else if (ChoiceName == "Run Away") {
            const int n = randomNum(1, 10);

            if (n <= 5) {
                cout << "You escape through the brambles, but the forest takes a little blood as payment." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, 10);
                cout << "You took 10 damage." << endl;
            } else {
                cout << "You slip away without being wounded. The whispers continue behind you, disappointed." << endl;
            }

        } else if (ChoiceName == "Explore the Basement") {
            cout << "You descend beneath the cathedral. Roots hang from the ceiling like old veins." << endl;

            const int n = randomNum(1, 10);

            if (n <= 7) {
                cout << "A Skeleton appears!" << endl;
                encounterSkeleton(playerIP, musicManager);
            }

            if (playerIP.health <= 0) {
                return;
            }

            if (n <= 2) {
                cout << "Another Skeleton appears!" << endl;
                encounterSkeleton(playerIP, musicManager);
            }

            if (playerIP.health <= 0) {
                return;
            }

            cout << "Behind a cracked stone panel, you find a strange amulet with a silver rim and a central sapphire." << endl;
            cout << "Tiny wisps flow and spin inside the gem like trapped weather." << endl;

            playerIP.maxSoul += 1;
            cout << "Your Max Souls increased by 1." << endl;

            ArmagestaBiomeTools::unlockCombatAction(
                playerIP,
                "Break Sapphire Amulet",
                "Break the sapphire amulet in combat, releasing the wisps trapped inside. Expensive, strange, and probably dangerous."
            );

        } else if (ChoiceName == "Appreciate the Stained Glass Artwork") {
            cout << "You stand before the stained glass and let the colors fall across you." << endl;
            cout << "One pane depicts a floral sword buried hilt-deep in a king's shadow." << endl;
            cout << "The image fills you with determination, then suddenly shatters." << endl;

            playerIP.permanentDamageModifier += 1;
            cout << "Your permanent damage increased by 1." << endl;

        } else if (ChoiceName == "Pray at the Altar") {
            const int n = randomNum(1, 10);

            cout << "There is a large brown book on the altar. Its pages turn without wind." << endl;

            if (n <= 7) {
                cout << "A warm, familiar soul rises out of the book and presses itself gently into your chest." << endl;
                cout << "You gained 5 Souls, and your Soul capacity feels larger." << endl;

                playerIP.maxSoul += 2;
                playerIP.gainSoul(5);
            } else {
                cout << "A large soul with demonic energy radiating from it emerges from the book." << endl;
                cout << "It pounces at you, dissipating into your chest like a curse pretending to be a blessing." << endl;

                playerIP.maxSoul += 2;
                playerIP.maxHealth -= 5;

                if (playerIP.health > playerIP.maxHealth) {
                    playerIP.health = playerIP.maxHealth;
                }

                cout << "Your Max Souls increased by 2, but your Max Health decreased by 5." << endl;
            }

        } else if (ChoiceName == "Move On") {
            cout << "Best leave that for another day." << endl;
            cout << "You feel slightly better knowing this was your choice." << endl;

            playerIP.heal(3);
            cout << "You healed 3 Health." << endl;

        } else if (ChoiceName == "Talk to the Tree") {
            const int n = randomNum(1, 10);

            cout << "The tree talks back. Its voice sounds like wood bending in winter." << endl;

            if (n <= 5) {
                cout << "The tree does not seem interested in conversation, but it drops a few apples for you." << endl;
                playerIP.heal(5);
                cout << "You healed 5 Health." << endl;
            } else if (n <= 7) {
                cout << "The tree becomes irrationally angry and bumps you away like a bug." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, 5);
                cout << "You took 5 damage." << endl;
            } else {
                cout << "The tree describes itself as an Ent and claims to have watched the first roads rot into moss." << endl;
                cout << "It gives you a hardwood stick from the top of its head and bids you off." << endl;

                ArmagestaBiomeTools::unlockCombatAction(
                    playerIP,
                    "Ent Wand",
                    "Call roots up from beneath the enemy, stabbing and binding them. This requires a large amount of Souls."
                );
            }

        } else if (ChoiceName == "Climb the Tree") {
            cout << "How many meters do you want to climb? Choose a number from 1 to 100." << endl;

            int metersToClimb = 0;
            string climbInput;

            while (true) {
                getline(cin >> ws, climbInput);

                try {
                    metersToClimb = stoi(climbInput);
                } catch (...) {
                    metersToClimb = 0;
                }

                if (metersToClimb > 0 && metersToClimb <= 100) {
                    break;
                }

                cout << "You can't even climb that high..." << endl;
                cout << "Choose a number from 1 to 100." << endl;
            }

            const int n = randomNum(1, metersToClimb);

            if (n <= 10) {
                cout << "You find an orange hidden between two branches." << endl;
                playerIP.heal(5);
                cout << "You healed 5 Health." << endl;
            } else if (n <= 30) {
                cout << "You find a large Blood Orange pulsing gently under the leaves." << endl;
                playerIP.maxHealth += 5;
                playerIP.heal(15);
                cout << "Your Max Health increased by 5, and you healed 15 Health." << endl;
            } else if (n <= 90) {
                const int damageTaken = randomNum(1, 30);

                cout << "The bark shifts under your hand. You fall hard through branches and leaves." << endl;
                ArmagestaBiomeTools::hurtPlayer(playerIP, damageTaken);
                cout << "You took " << damageTaken << " damage." << endl;
            } else {
                cout << "At the top of the tree, it feels like you can see the entire world." << endl;
                cout << "The map of Armagesta opens in your mind like a wound made of roads." << endl;
                playerIP.printMap();
            }

        } else if (ChoiceName == "Carve into the Tree") {
            cout << "The blade bites into bark." << endl;
            cout << "The tree awakens, and it is furious." << endl;

            // If your Ent function is named encounterEnt instead, replace this line with:
            // encounterEnt(playerIP, musicManager);
            encounterAncientEnt(playerIP, musicManager);

            if (playerIP.health <= 0) {
                return;
            }

            cout << "From the corpse of the Ent, you take a large piece of unbreakable bark." << endl;
            cout << "It melds to your arm, becoming shield, scar, and warning." << endl;

            playerIP.hardiness += 5;
            playerIP.heal(30);

            cout << "Your Hardiness increased by 5, and you healed 30 Health." << endl;
        }
    }

public:
    explicit Forest(Player& PlayerIP, BackgroundMusicManager& musicManager)
        : BiomeEventDatabase(PlayerIP, musicManager) {
        REI = getChoice();
        doEvent();
    }
};

class Desert : public BiomeEventDatabase {
private:
    vector<int> eventChances = {25, 20, 15, 15, 10, 10, 5};
    vector<string> eventNames = {"Sun-Buried Road", "Obsidian Caravan", "Mirage Court", "Dune Maw Sign", "Buried Saint", "Storm of Singing Sand", "Black Pyramid Far Away"};
    vector<string> eventFlavorText = {
        "A road of half-buried white stones cuts through the dunes, appearing and disappearing wherever the wind chooses to uncover it. Every stone is engraved with the same warning: DO NOT TRUST SHADE. The letters are not chipped by age, which means either someone maintains them, or the warning is recent enough to still be bleeding meaning.",
        "A caravan of black wagons rolls without horses, moving smoothly over the dunes as if pulled by an agreement rather than reins. Its wheels leave no tracks, and the merchants wave with the bored patience of people who expected you hours ago. Their faces are hidden behind blue veils, but their shadows are clearly counting your pockets.",
        "A palace of heat and blue silk trembles in the distance, beautiful enough to make thirst feel like devotion. Its open gates reveal your own reflection sitting on a throne of water, wearing a crown made from everything you have ever wanted. The mirage does not flicker when you approach; instead, the real world does.",
        "The dune ahead rises and falls like something asleep beneath a blanket of gold. Broken shields circle it in a wide ring, their splintered edges arranged like teeth around a mouth too patient to open yet. The sand near your boots trembles in time with a buried heartbeat, and the desert becomes very interested in whether you step lightly.",
        "A stone hand reaches from the sand, still clutching a sun-shaped ring that has not dulled under centuries of heat. The rest of the saint lies buried below, body, story, and miracle all packed beneath a weight of thirst. Small beetles crawl in prayerful circles around the fingers, as if even insects remember this person better than the living do.",
        "The wind begins to sing with hundreds of dry voices, each grain of sand carrying a syllable stolen from a dead mouth. Some voices beg for water, some laugh at kingdoms, and one unmistakably belongs to you. The song moves in circles around your skull, trying to teach your feet a dance that ends beneath the dunes.",
        "For one moment the horizon clears, revealing a black pyramid where no map admits anything should stand. Its sides drink the sunlight instead of reflecting it, and a single red door burns at the base like an eye held open. Then the heat closes again, but the shape remains behind your eyelids, waiting to be believed."
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
        "The cave entrance exhales mineral-cold air over your face, steady as the breath of a sleeping giant. It smells like coins, rain, and something pale that has never seen the sun yet knows how to hate it. Pebbles roll inward without being touched, as if the earth is swallowing slowly and politely.",
        "A black river moves through the tunnel without reflecting your torch, your face, or the ceiling above it. Small pale fish swim upstream with human teeth, snapping at bubbles and whispering through their gills. The current looks shallow near the bank and bottomless in the middle, which feels less like geography and more like a trap with manners.",
        "The tunnel ends at a gate made from the ribcage of a creature large enough to make mountains feel young. Each bone is carved with tiny scenes of miners, kings, and nameless things crawling up from below. The gate does not block the path so much as ask whether you understand what kind of body you are entering.",
        "Crystals hang from the ceiling in perfect rows, clear as frozen bells and sharp enough to split a careless thought. When you breathe, they answer in harmony, each note returning a slightly altered version of your heartbeat. The cave listens through them, and the music suggests there are rooms below that open only for the correctly wounded.",
        "Webbing thick as rope knots the walls, tying stalactites to skulls and abandoned tools to one another. You can hear many legs stopping whenever you stop, then beginning again a heartbeat after you move. The silence between those movements is worse than the skittering, because it proves the nest is thinking.",
        "A wooden lift hangs over a vertical shaft, swaying gently though no wind reaches this far underground. Its rope is newer than the cave around it, which is not comforting; someone has been using this path recently, and they did not bother leaving a warning. From below comes the faint scrape of machinery, or claws learning to imitate it.",
        "An ancient forge glows under the stone, still hot without coal, bellows, or any mercy from time. Hammers strike somewhere below in a rhythm too perfect for living hands, shaping metal you cannot see into laws you may have to obey. The walls are black with soot, but beneath it are royal crests from kingdoms that never reached the surface."
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
        "The redwoods rise so high that the sky feels like a rumor told by leaves to creatures too small to verify it. Their trunks are wider than houses and scarred by fires that must have burned before your bloodline had a name. Every footstep sinks into centuries of needles, and the forest absorbs the sound as if adding you to its archive.",
        "Three trees lean inward, their bark twisted into faces that seem carved by patience rather than tools. They do not speak, but the roots around your feet tighten like a courtroom drawing its doors shut. Sap gathers in the grooves of their eyes, amber and slow, as if verdicts in this place are measured by seasons.",
        "Golden amber drips from wounds in the branches, falling in bright beads that never quite strike the ground. Inside each drop, something tiny is preserved mid-scream: insects, teeth, little scraps of banner cloth, perhaps a thought that escaped too late. The whole grove smells sweet and funereal, like a memory being sealed before it can defend itself.",
        "A circle of redwoods bears axe scars wider than doors, each cut healed over in thick ridges like clenched muscle. The forest remembers the war and has not forgiven the metal that made it, nor the hands that praised the metal for being useful. Moss grows in the wounds in the shape of old battle maps, all of them ending badly for men with crowns.",
        "Young ents no taller than you pretend to be ordinary saplings, freezing whenever you look directly at them. They are terrible actors; one still has a squirrel in its branch-hair, and another keeps slowly inching away from your shadow. Their playfulness is almost comforting until you remember that even children in this forest can learn anger from the roots.",
        "Thousands of wooden bells hang from threads of moss, each one carved from a different tree and marked with a name. None move in the still air, but all of them ring inside your chest, soft blows against the ribs from the inside. The sound is not loud; it is intimate, like the forest testing whether grief can be used as a compass.",
        "A single red root crosses the path, pulsing like a vein from one end of the forest to the other. It disappears beneath trunks, stones, and old graves, binding the grove together with something warmer than sap. Standing near it, you feel the Old Redwoods breathe as one body, and you understand that cutting one tree may wound them all."
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
        "A ferry waits in brown water with no ferryman, its boards slick with moss and old handprints. The pole moves by itself, tapping impatiently against the mud like a finger on a table. Across the water, fog gathers into the rough outline of a shore that may not have been there a moment ago.",
        "A chapel sinks at an angle into the swamp, its stained glass half-submerged and still glowing in dull colors beneath the waterline. Mosquitoes gather around its steeple like worshippers, humming a hymn made entirely of hunger. The cross at the roof has been replaced by a bundle of reeds tied in the shape of a warning.",
        "Blue lights bob between the trees, vanishing whenever they pass behind trunks too thin to hide them. They move like lanterns carried by people who do not want to be seen, or by things pretending to understand how people carry lanterns. Their glow turns every puddle into an eye, and every eye seems pleased you noticed.",
        "A still pool bubbles once, then twice, then in a rhythm too much like breathing to dismiss as rot. The mud around it is stamped flat by many heavy bodies, though no trail leads away. Something below the surface shifts with the slow confidence of a creature that has mistaken the whole swamp for its throat.",
        "Rusty helmets and broken banners poke from the mud, each one slowly sinking and rising with the swamp's breath. The battle here never ended; it merely became wetter, quieter, and more patient. Some helmets turn slightly as you pass, as if the soldiers inside are still waiting for a command no living officer remembers.",
        "A black lotus blooms in water so dark it reflects your face older than it is. Its petals open and close with the rhythm of a sleeping mouth, releasing a perfume that tastes like regret on the back of your tongue. Around it, dead frogs float belly-up in a perfect circle, honored or sacrificed.",
        "The mud repeats your footsteps after you stop moving, soft presses in the muck arriving one by one behind you. Something below is learning you: your weight, your rhythm, the pauses you make when afraid. The reeds lean closer, not because of wind, but because secrets in the swamp are communal property."
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
        "The Citadel gate is raised just enough for one person to crawl beneath it, forcing any visitor to enter bowed and dirt-streaked. That feels less like welcome and more like digestion, as if the fortress prefers to swallow the humble first. Above you, murder holes stare down in neat rows, patient as bureaucrats.",
        "Rows of bunks wait in perfect order, untouched by dust except for the prints of boots no one is wearing. Every pillow has a sword laid on it, like the soldiers left during the same dream and expected to return before waking. The room smells faintly of oil, iron, and discipline that outlived the people it was meant to protect.",
        "A marble hall stretches forward, lined with statues of citizens begging a king whose face has been chiseled away. Their stone hands reach with such careful desperation that you can almost hear the petitions they died repeating. At the far end, the empty royal seat is covered in claw marks, suggesting the last appeal was answered by something larger than law.",
        "Iron cells contain no prisoners, only names carved into the walls in thousands of different hands. Some names are still breathing, the letters swelling and shrinking like lungs trapped in stone. The Citadel apparently learned that a body can escape more easily than a sentence.",
        "A chapel furnace glows beneath a cracked altar, bathing the pews in red light instead of holiness. Here, faith and metal were apparently the same craft; prayers were hammered, quenched, sharpened, and sent to war. The ash in the offering bowls is full of tiny melted crowns.",
        "Stone gargoyles crouch above the walkway, their claws dug deep into the masonry and their mouths open in silent alarm. They all face inward toward the throne instead of outward toward enemies, which says something terrible about what the Citadel feared most. When you pass beneath them, grit falls from their eyes like old tears or fresh attention.",
        "Weapon racks cover the room, but most blades are fused to the stone by royal decree, punished for being drawn by the wrong hands. One rack still trembles, metal shivering against metal as if some sword inside it dreams of disobedience. The air tastes like old commands, and your own weapon feels heavier in response."
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
        "The tide pools hold small trapped worlds, each one clear enough to show a different version of the sky. In one, a tiny storm rages over a tiny sea; in another, a moon rises where no moon should fit. Crabs and minnows move through these worlds like citizens of kingdoms that will vanish when the tide remembers them.",
        "A shipwreck lies cracked open like a whale skeleton, ribs of timber jutting from the sand and black kelp hanging like torn sailcloth. Its mast points inland, accusing the mountains as though stone, not storm, dragged it here. The name on the hull has been scratched away, but the scratches are fresh enough to sting.",
        "Pearls are arranged in rows beneath driftwood markers, each one polished by waves until it looks like a small sleeping eye. Someone buried their dead with the ocean's teeth, and the tide keeps returning to check the graves. There are no bodies visible, only offerings, which makes the shore feel less empty and more restrained.",
        "The surf glows under moonlight even though it is day, silver lines crawling across the water beneath a sun that pretends not to notice. Waves arrive out of order, some young and foaming, some old and heavy with memories from storms not yet born. Standing at the edge, you feel time pulling at your ankles like undertow.",
        "A shrine of salt blocks stands where the waves cannot quite reach, though every gust carries enough spray to keep it glittering. Offerings dissolve before your eyes: coins, flowers, prayers written on thin paper, all reduced to taste and absence. The shrine seems less dedicated to a god than to the certainty that everything returns to water.",
        "Glass-shelled crabs march in a military line, each carrying a stolen button, coin, or bone chip beneath its transparent armor. Their formation is too precise to be instinct and too absurd to be truly disciplined. At their center, one crab carries a tiny rusted crown and keeps stopping to make sure the others bow.",
        "A lighthouse leans over the shore, its stones cracked by salt and stubbornness, yet its lamp still turns. The beam illuminates things the sun politely ignores: footprints walking backward into the water, shadows without owners, a door painted on the side of a wave. Every revolution of the light makes the world feel briefly more honest and therefore more dangerous."
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
        "The path narrows until the mountain seems to be deciding which side of you to keep. One wall rises sheer and ice-dark beside you, while the other drops into clouds that hide the bottom out of either mercy or pride. Each step knocks loose a few pebbles, and their long fall counts the cost of carelessness for you.",
        "Snow fills a wide stone bowl surrounded by cliffs bright enough to hurt your eyes. Every sound you make comes back larger and more dangerous, returned by the slopes as an accusation. Beneath the snow, old boulders shift in their sleep, waiting for one loud mistake to become permission.",
        "A thread of smoke rises from a hut wedged between impossible rocks, curling upward in a line too straight for weather. Someone lives here by argument with gravity, stubbornness, and perhaps a treaty with the storm. The door is small, the footprints are large, and the cooking smell is either soup or spellwork.",
        "An altar of blue stone stands above the clouds, its surface polished smooth by snow, knees, and sacrifice. Birds avoid flying over it, bending around the place as if the sky itself has declared a boundary. Old runes around the edge name winds, kings, dragons, and one word that has been chiseled out with deliberate fear.",
        "A dragon skeleton coils around the ridge, ribs rising from the snow like the ruins of a cathedral built by hunger. Its skull is large enough to hold a chapel inside the jaw, and someone has actually placed candles between the teeth. The bones are dead, but the air around them is warm, as if the mountain has not finished digesting the fire.",
        "A watchtower stands frozen shut, windows filmed with ice thick enough to blur whatever moves behind them. The guards inside are still at their posts, or something shaped like guards is, standing too straight for corpses and too still for the living. A banner above the door cracks in the wind, showing the Citadel crest burned through by dragonfire.",
        "The air grows thin enough that each breath feels borrowed from something that expects repayment. Your vision sharpens at the edges and softens in the center, turning the world into a blade wrapped in fog. The mountain waits to see what you will spend your borrowed breath on: caution, prayer, or one more step upward."
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
                if (playerIP.health > 0) {
                    playerIP.markDragonDefeated();
                }
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
                if (playerIP.health > 0) {
                    playerIP.markDragonDefeated();
                }
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
    vector<string> eventFlavorText = {"The sand here pours downward forever, not into a hole, but into a decision that the desert has been postponing for ages. Every grain whispers the same word: descend, and the whisper becomes louder the longer you resist. Shields, bones, and broken ropes circle the pit, proof that many people heard the invitation and mistook it for destiny."};
    vector<vector<string>> eventChoices = {{"Descend", "Throw Scrap First", "Meditate at the Edge", "Leave"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("Sand Pit - Unfinished Descent", "The pit refuses to offer meditation, trade, or retreat a second time. The softer paths have collapsed into the sand behind you, leaving only the old hunger below. Every grain now falls faster, as if the Dune Maw has learned your name and grown impatient with ceremony.");
        vector<string> loreChoices = {"Descend"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("Sand Pit");
        if (ChoiceName == "Descend") {
            cout << "You slide into the pit and learn that falling can be steered if you stop arguing with gravity." << endl;
            if (loreOnlyRevisit || !playerIP.hasStoryFlag("Dune Maw Defeated")) {
                encounterDuneMaw(playerIP, musicManager);
                if (playerIP.health > 0) {
                    playerIP.addStoryFlag("Dune Maw Defeated");
                    if (!ArmagestaBiomeTools::hasCombatAction(playerIP, "Sandstep")) {
                        ArmagestaBiomeTools::unlockCombatAction(playerIP, "Sandstep", "Spend 2 Souls to dodge with desert footwork, gain Momentum, and raise defense for the turn.");
                    }
                }
            } else {
                cout << "The Dune Maw is already dead. The pit recognizes your footwork and lets you ride the sand safely." << endl;
                if (!loreOnlyRevisit) {
                    playerIP.heal(12);
                }
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
    explicit SandPit(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = SandPit::getChoice();
        if (this->loreOnlyRevisit) {
            SandPit::doLoreOnlyEvent();
        } else {
            SandPit::doEvent();
        }
    }
};

class CrystalGeode : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Crystal Geode"};
    vector<string> eventFlavorText = {"The cave opens into a perfect crystal heart, vast and faceted, beating without flesh at the center of the stone. Light grows here like moss, spreading over every surface in quiet colors that seem to have memories attached. Each shard reflects a version of you that survived something different, and some of those versions look disappointed that you made it this far."};
    vector<vector<string>> eventChoices = {{"Touch the Heart Crystal", "Break a Reflection", "Kneel in the Light", "Leave Carefully"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("Crystal Geode - Broken Reflection", "The harmless light no longer answers you. The kinder reflections have gone dark, and only the shard with teeth still shines. The Crystal Matriarch waits behind your face, tapping from the other side as if the mirror were an eggshell.");
        vector<string> loreChoices = {"Break a Reflection"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("Crystal Geode");
        if (ChoiceName == "Touch the Heart Crystal") {
            cout << "The heart crystal grows around your hand, then decides you are not worth keeping." << endl;
            if (!ArmagestaBiomeTools::hasCombatAction(playerIP, "Crystal Ward")) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Crystal Ward", "Spend 3 Souls to create a crystal defense wall. If an enemy attacks, broken light damages it back.");
                playerIP.hardiness += 1;
            } else {
                playerIP.heal(15);
            }
        } else if (ChoiceName == "Break a Reflection") {
            if (playerIP.hasStoryFlag("Crystal Matriarch Defeated")) {
                cout << "The Crystal Matriarch is already broken. The geode has no second queen to offer." << endl;
                return;
            }
            cout << "The broken reflection crawls out as the Crystal Matriarch's anger." << endl;
            encounterCrystalMatriarch(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.addStoryFlag("Crystal Matriarch Defeated");
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
    explicit CrystalGeode(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = CrystalGeode::getChoice();
        if (this->loreOnlyRevisit) {
            CrystalGeode::doLoreOnlyEvent();
        } else {
            CrystalGeode::doEvent();
        }
    }
};

class OldestRedwood : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Oldest Redwood"};
    vector<string> eventFlavorText = {"This redwood is not a tree so much as a vertical country, with ridges like roads and hollows large enough to shelter forgotten villages. Its bark contains closed doors, old faces, and one heartbeat too slow for human fear. The roots spread beneath the forest like a sleeping council, and every lesser tree seems to lean toward this one for orders."};
    vector<vector<string>> eventChoices = {{"Speak the Old Oath", "Climb Inside the Bark", "Challenge the Elder", "Sleep at its Roots"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("Oldest Redwood - Elder Challenge", "The roots close around every lesser path, sealing off fruit, conversation, and mercy with a slow wooden groan. The ancient tree offers only its oldest test now. Somewhere high above, leaves turn their pale undersides toward you like thousands of watching eyes.");
        vector<string> loreChoices = {"Challenge the Elder"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("Oldest Redwood");
        if (ChoiceName == "Speak the Old Oath") {
            cout << "You do not know the words until the tree says them through you." << endl;
            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Redwood Oath", "Spend 5 Souls to heal, gain defense, empower your attacks, and weaken the enemy with ancient roots.");
            playerIP.maxHealth += 5;
        } else if (ChoiceName == "Climb Inside the Bark") {
            cout << "Inside the tree are rooms grown for people who never arrived." << endl;
            playerIP.maxSoul += 2;
            playerIP.printMap();
        } else if (ChoiceName == "Challenge the Elder") {
            if (playerIP.hasStoryFlag("Ancient Ent Defeated")) {
                cout << "The Ancient Ent has already fallen. The Oldest Redwood will not grow another for your convenience." << endl;
                return;
            }
            cout << "The Oldest Redwood does not fight you itself. It sends an Ancient Ent like a raised eyebrow." << endl;
            encounterAncientEnt(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.addStoryFlag("Ancient Ent Defeated");
                playerIP.hardiness += 3;
            }
        } else if (ChoiceName == "Sleep at its Roots") {
            cout << "You sleep for one night and dream for one century." << endl;
            playerIP.heal(30);
            playerIP.gainSoul(5);
        }
    }
public:
    explicit OldestRedwood(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = OldestRedwood::getChoice();
        if (this->loreOnlyRevisit) {
            OldestRedwood::doLoreOnlyEvent();
        } else {
            OldestRedwood::doEvent();
        }
    }
};

class WitchsHut : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Witch's Hut"};
    vector<string> eventFlavorText = {"The hut stands on stilts made of bird bones and bad decisions, rocking gently above the swamp without touching the mud. Smoke leaks from the chimney in the shape of question marks, then hooks itself into the clouds as if fishing for answers. Charms made of teeth, spoons, and tiny masks clatter at the door, each one eager to warn you too late."};
    vector<vector<string>> eventChoices = {{"Knock Three Times", "Drink from the Cauldron", "Buy a Hex", "Steal the Broom"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("Witch's Hut - Stolen Broom", "The cauldron goes cold and the shop shelves shut themselves, denying every small bargain you might have used to stall. The hut remembers what remains unfinished, and the floorboards creak in the direction of the broom. Outside, the swamp holds its breath for the moment theft becomes war.");
        vector<string> loreChoices = {"Steal the Broom"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("Witch's Hut");
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
            if (playerIP.hasStoryFlag("Marionette Coven Defeated")) {
                cout << "The Marionette Coven has already been cut down. The broom lies quiet." << endl;
                return;
            }
            cout << "The broom screams for its sisters. The Marionette Coven answers instead." << endl;
            encounterMarionetteCoven(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.addStoryFlag("Marionette Coven Defeated");
            }
        }
    }
public:
    explicit WitchsHut(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = WitchsHut::getChoice();
        if (this->loreOnlyRevisit) {
            WitchsHut::doLoreOnlyEvent();
        } else {
            WitchsHut::doEvent();
        }
    }
};

class TheThrone : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"The Throne"};
    vector<string> eventFlavorText = {"The throne room has no roof, only broken arches reaching up like fingers that failed to hold the heavens in place. The sky itself looks down like a witness, cold and unblinking, while ash gathers in the seams of the marble floor. The throne waits, empty and offended, as if abandonment were the greatest treason ever committed against it."};
    vector<vector<string>> eventChoices = {{"Sit on the Throne", "Break the Crown", "Read the Law", "Bow and Leave"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("The Throne - Final Claim", "The lesser laws are silent now. The Dragon is dead, the old fear has lost its throne, and the room permits only the question of rulership. Every cracked pillar seems to lean inward, waiting to see whether you are a monarch, a mistake, or merely the next person to sit too soon.");
        vector<string> loreChoices = {"Sit on the Throne"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("The Throne");
        if (ChoiceName == "Sit on the Throne") {
            cout << "The throne tries to decide whether you are a ruler, a thief, or fuel." << endl;
            if (!playerIP.hasStoryFlag("Starless Oracle Defeated")) {
                encounterStarlessOracle(playerIP, musicManager);
                if (playerIP.health <= 0) {
                    return;
                }
                playerIP.addStoryFlag("Starless Oracle Defeated");
            } else {
                cout << "The Starless Oracle has already judged you. Its silence is permission enough." << endl;
            }

            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Royal Decree", "Spend 3 Souls and 2 Momentum to forbid an enemy attack from landing, or condemn a passive enemy.");
            if (!loreOnlyRevisit && !playerIP.throneClaimed) {
                playerIP.maxSoul += 1;
            }
            playerIP.claimThrone();
        } else if (ChoiceName == "Break the Crown") {
            cout << "The crown breaks like a chain. The Citadel shakes with relief and rage." << endl;
            encounterReforgedKnight(playerIP, musicManager);
            playerIP.permanentDamageModifier += 2;
        } else if (ChoiceName == "Read the Law") {
            cout << "The first law is survival. The second is obedience. The third has been scratched out by many hands." << endl;
            ArmagestaBiomeTools::unlockCombatAction(playerIP, "Royal Decree", "Spend 3 Souls and 2 Momentum to forbid an enemy attack from landing, or condemn a passive enemy.");
            playerIP.addQuest("Defeat the Cinder Dragon, then return to The Throne");
        } else if (ChoiceName == "Bow and Leave") {
            cout << "The empty throne accepts respect better than occupation." << endl;
            playerIP.heal(20);
            playerIP.gainSoul(3);
        }
    }
public:
    explicit TheThrone(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = TheThrone::getChoice();
        if (this->loreOnlyRevisit) {
            TheThrone::doLoreOnlyEvent();
        } else {
            TheThrone::doEvent();
        }
    }
};

class CoralReef : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Coral Reef"};
    vector<string> eventFlavorText = {"The reef glows beneath clear water like a city that chose to become an animal and learned to breathe in color. Currents twist around it in careful, deliberate sentences, carrying flecks of pearl, bone, and blue light through streets of coral. From the deepest part comes a pulse that makes the surface tremble, as though the ocean has a heart and resents being discovered."};
    vector<vector<string>> eventChoices = {{"Dive to the Heart", "Take a Coral Crown", "Listen to the Current", "Surface"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("Coral Reef - Heart Dive", "The current closes every shallow path, sweeping aside shells, pearls, and every easy excuse to remain near the surface. The reef has saved only its deepest answer for you. Below, the Tide Leviathan turns once in the blue dark, and the whole ocean shifts around its patience.");
        vector<string> loreChoices = {"Dive to the Heart"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("Coral Reef");
        if (ChoiceName == "Dive to the Heart") {
            if (playerIP.hasStoryFlag("Tide Leviathan Defeated")) {
                cout << "The Tide Leviathan is already defeated. The reef's heart beats quietly around the empty altar." << endl;
                return;
            }
            cout << "At the reef's heart, the Tide Leviathan coils around an altar of pearls." << endl;
            encounterTideLeviathan(playerIP, musicManager);
            if (playerIP.health > 0) {
                playerIP.addStoryFlag("Tide Leviathan Defeated");
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
    explicit CoralReef(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = CoralReef::getChoice();
        if (this->loreOnlyRevisit) {
            CoralReef::doLoreOnlyEvent();
        } else {
            CoralReef::doEvent();
        }
    }
};

class DragonsLair : public BiomeEventDatabase {
private:
    vector<int> eventChances = {100};
    vector<string> eventNames = {"Dragon's Lair"};
    vector<string> eventFlavorText = {"The lair is not inside the mountain; the mountain is inside the lair, hollowed around an absence so hot it has become architecture. Gold, bones, and old fire make a nest large enough for the end of a story, and every treasure here looks less owned than consumed. Far above, the stone ceiling glows red in slow pulses, matching the breath of something that has mistaken the world for its inheritance."};
    vector<vector<string>> eventChoices = {{"Challenge the Dragon", "Steal an Ember", "Offer the Hoard", "Retreat"}};
    string ChoiceName;
    int REI = -1;
    bool loreOnlyRevisit = false;
protected:
    int getChoice() override { return ArmagestaBiomeTools::weightedChoice(eventChances); }
    void doEvent() override {
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
        ResolveChoice();
    }
    void doLoreOnlyEvent() {
        ArmagestaBiomeTools::printEventHeader("Dragon's Lair - Cinder Reckoning", "The hoard will not bargain, the ember will not be stolen, and retreat has already had its chance. The lair has stripped away every side path, leaving only heat, gold, and the shape of the Dragon in the smoke. This is no longer exploration; this is the mountain asking whether your story deserves an ending.");
        vector<string> loreChoices = {"Challenge the Dragon"};
        ChoiceName = loreChoices[ArmagestaBiomeTools::askChoice(loreChoices)];
        ResolveChoice();
    }
    void ResolveChoice() override {
        playerIP.discoverLandmark("Dragon's Lair");
        if (ChoiceName == "Challenge the Dragon") {
            if (playerIP.dragonDefeated) {
                cout << "The Cinder Dragon is already dead. The mountain keeps the shape of its absence." << endl;
                return;
            }
            cout << "The Cinder Dragon opens one eye. It has been awake for years, waiting for someone rude enough to call this a beginning." << endl;
            encounterCinderDragon(playerIP, musicManager);
            if (playerIP.health > 0) {
                ArmagestaBiomeTools::unlockCombatAction(playerIP, "Dragonfire", "Spend 8 Souls to breathe stolen dragonfire for massive damage, burning yourself in the process.");
                playerIP.maxSoul += 2;
                playerIP.permanentDamageModifier += 2;
                playerIP.markDragonDefeated();
            }
        } else if (ChoiceName == "Steal an Ember") {
            cout << "The ember burns through the idea of your hand before reaching the skin." << endl;
            ArmagestaBiomeTools::hurtPlayer(playerIP, 15);
            bool foughtDragonHere = false;
            if (randomNum(1, 10) <= 5) {
                foughtDragonHere = true;
                encounterCinderDragon(playerIP, musicManager);
            }
            if (foughtDragonHere && playerIP.health > 0) {
                playerIP.markDragonDefeated();
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
    explicit DragonsLair(Player& PlayerIP, BackgroundMusicManager& musicManager, const bool loreOnlyRevisit = false) : BiomeEventDatabase(PlayerIP, musicManager) {
        this->loreOnlyRevisit = loreOnlyRevisit;
        REI = DragonsLair::getChoice();
        if (this->loreOnlyRevisit) {
            DragonsLair::doLoreOnlyEvent();
        } else {
            DragonsLair::doEvent();
        }
    }
};

// Template for Biomes

class NEWBIOMENAME : public BiomeEventDatabase {
private:
    vector<int> eventChances = {70, 30};
    vector<string> eventNames = {"EVENT 1", "EVENT 2"};
    vector<string> eventFlavorText = {
        "Write two to four sentences here that establish the place, the mood, and why the choice matters. Include one concrete sensory detail, one strange or unsettling image, and one hint about the lore or danger connected to this event.",
        "Write two to four sentences here that show how this event changes the player's understanding of the biome. The description should give the player a reason to care before the choices appear. Try to imply a mechanical risk or reward through the fiction instead of explaining the numbers directly."
    };
    vector<vector<string>> eventChoices = {
        {"CHOICE 1", "CHOICE 2"},
        {"CHOICE 1", "CHOICE 2"}
    };

    string ChoiceName;

    int REI = -1;

protected:
    int getChoice() override {
        return ArmagestaBiomeTools::weightedChoice(eventChances);
    }

    void doEvent() override{
        ArmagestaBiomeTools::printEventHeader(eventNames[REI], eventFlavorText[REI]);
        ChoiceName = eventChoices[REI][ArmagestaBiomeTools::askChoice(eventChoices[REI])];
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
