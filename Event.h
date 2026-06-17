//
// Created by Taylor Anderson on 6/15/26.
//

#ifndef ARMAGESTA_EVENT_H
#define ARMAGESTA_EVENT_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include "EventDatabase.h"
#include <vector>
#include "PlayerDatabase.h"


using namespace std;

class Event {
private:
    const string biome;
    shared_ptr<BiomeEventDatabase> biomeIP;
    string nameOfEvent;
    string eventText;
    vector<string> eventChoices;
    vector<string> eventConsequences;
    shared_ptr<Player> playerIP;

public:
    explicit Event(string biomeName, shared_ptr<BiomeEventDatabase> newBiomeIP, shared_ptr<Player> PlayerIP)
    : biome(std::move(biomeName)), biomeIP(std::move(newBiomeIP)), playerIP(std::move(PlayerIP)){

    };
};



#endif //ARMAGESTA_EVENT_H
