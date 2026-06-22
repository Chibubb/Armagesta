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

inline bool playSoundEffect(const std::string& filePath1, const std::string& filePath2, const std::string& filePath3) {
    static sf::SoundBuffer buffer;
    static std::optional<sf::Sound> sound;

    // Stop the previous sound effect if one is already playing
    if (sound.has_value()) {
        sound->stop();
        sound.reset();
    }

    // Load the sound file
    int randTrack = randomNum(1, 3);

    switch (randTrack) {
        case 1:
            if (!buffer.loadFromFile(filePath1)) {
                std::cout << "Failed to load sound effect: " << filePath1 << std::endl;
                return false;
            }
            break;
        case 2:
            if (!buffer.loadFromFile(filePath2)) {
                std::cout << "Failed to load sound effect: " << filePath2 << std::endl;
                return false;
            }
            break;
        case 3:
            if (!buffer.loadFromFile(filePath3)) {
                std::cout << "Failed to load sound effect: " << filePath3 << std::endl;
                return false;
            }
            break;
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
    //Can be "M-ATTACK", "R-ATTACK", or "NO TYPE"
    string type = "NO TYPE";
    string attackSpeed = "Normal";
    int defenceValue = 0;
};

#endif //ARMAGESTA_RANDOMNESS_H
