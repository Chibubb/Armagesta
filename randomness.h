//
// Created by Taylor Anderson on 6/15/26.
//

#ifndef ARMAGESTA_RANDOMNESS_H
#define ARMAGESTA_RANDOMNESS_H
#include <vector>
#include <random>
#include <SFML/Audio.hpp>
#include <iostream>
#include <optional>
#include <string>

using namespace std;

inline int randomNum(const int min, const int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

inline void makeZeroIfNegative(int& value) {
    if (value < 0) {
        value = 0;
    }
}

inline bool playSoundEffect(const std::string& filePath) {
    static sf::SoundBuffer buffer;
    static std::optional<sf::Sound> sound;

    // Stop the previous sound effect if one is already playing
    if (sound.has_value()) {
        sound->stop();
        sound.reset();
    }

    // Load the sound file
    if (!buffer.loadFromFile(filePath)) {
        std::cout << "Failed to load sound effect: " << filePath << std::endl;
        return false;
    }

    // Create and play the sound
    sound.emplace(buffer);
    sound->setVolume(75.0f);
    sound->play();

    return true;
}


//attackSpeed can either be "Quick" "Normal" "Slow"

struct PAD {
    //Can be "MISSED HIT", "CRITICAL HIT", "ATTACK", "DEFENSE", "DODGE", "CHARGE", "PARRY", "MAGIC", or "NO TYPE"
    string type;
    string attackSpeed = "Normal";
    int defenceValue = 0;
};

struct MAD {
    //Can be "ATTACK" or "NO TYPE"
    string type = "NO TYPE";
    string attackSpeed = "Normal";
    int defenceValue = 0;
};

#endif //ARMAGESTA_RANDOMNESS_H
