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
#include <EventDatabase.h>

using namespace std;

class Event {
private:
    const string biome;
    const shared_ptr<int> biomeIP;
    string nameOfEvent;
    string eventText;
    vector<string> eventChoices;
    vector<string> eventConsequences;

public:
    explicit Event(string biomeName, shared_ptr<int> newBiomeIP) : biome(biomeName), biomeIP(newBiomeIP) {

    };
};



#endif //ARMAGESTA_EVENT_H
