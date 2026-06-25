//
// Created by Taylor Anderson on 6/21/26.
//

#ifndef ARMAGESTA_BACKGROUNDMUSICMANAGER_H
#define ARMAGESTA_BACKGROUNDMUSICMANAGER_H

#include <SFML/Audio.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

class BackgroundMusicManager {
private:
    sf::Music music;

    std::string currentTrack;
    float maxVolume = 35.0f;
    bool hasLoadedTrack = false;

    void fadeVolume(float startVolume, float endVolume, float seconds) {
        if (seconds <= 0.0f) {
            music.setVolume(endVolume);
            return;
        }

        using clock = std::chrono::steady_clock;
        const auto startTime = clock::now();

        while (true) {
            const auto now = clock::now();
            float elapsed = std::chrono::duration<float>(now - startTime).count();
            float t = elapsed / seconds;

            if (t > 1.0f) {
                t = 1.0f;
            }

            const float currentVolume = startVolume + (endVolume - startVolume) * t;
            music.setVolume(currentVolume);

            if (t >= 1.0f) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(24));
        }
    }

public:
    // MP3 files often pop at their loop boundary. Armagesta now defaults music to not looping,
    // which removes the repeating click while still allowing long ambience tracks to play normally.
    bool playMusicImmediately(const std::string& filePath, bool loop = false) {
        if (hasLoadedTrack && currentTrack == filePath) {
            return true;
        }

        music.setVolume(0.0f);
        music.stop();

        if (!music.openFromFile(filePath)) {
            std::cout << "Failed to load music file: " << filePath << std::endl;
            hasLoadedTrack = false;
            currentTrack = "";
            return false;
        }

        currentTrack = filePath;
        hasLoadedTrack = true;

        music.setLooping(loop);
        music.setVolume(maxVolume);
        music.play();

        return true;
    }

    bool changeMusicWithFade(
        const std::string& filePath,
        bool loop = false,
        float fadeOutSeconds = 0.35f,
        float fadeInSeconds = 0.35f
    ) {
        if (hasLoadedTrack && currentTrack == filePath) {
            return true;
        }

        if (hasLoadedTrack) {
            fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
            std::this_thread::sleep_for(std::chrono::milliseconds(35));
            music.stop();
        }

        if (!music.openFromFile(filePath)) {
            std::cout << "Failed to load music file: " << filePath << std::endl;
            hasLoadedTrack = false;
            currentTrack = "";
            return false;
        }

        currentTrack = filePath;
        hasLoadedTrack = true;

        music.setLooping(loop);
        music.setVolume(0.0f);
        music.play();

        fadeVolume(0.0f, maxVolume, fadeInSeconds);

        return true;
    }

    void stopMusicWithFade(float fadeOutSeconds = 0.35f) {
        if (!hasLoadedTrack) {
            return;
        }

        fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
        std::this_thread::sleep_for(std::chrono::milliseconds(35));
        music.stop();
        hasLoadedTrack = false;
        currentTrack = "";
    }

    void pauseMusic() {
        music.pause();
    }

    void resumeMusic() {
        if (hasLoadedTrack) {
            music.play();
        }
    }

    void setVolume(float newVolume) {
        maxVolume = std::clamp(newVolume, 0.0f, 100.0f);
        music.setVolume(maxVolume);
    }

    [[nodiscard]] float getVolume() const {
        return maxVolume;
    }

    [[nodiscard]] std::string getCurrentTrack() const {
        return currentTrack;
    }
};

#endif // ARMAGESTA_BACKGROUNDMUSICMANAGER_H
