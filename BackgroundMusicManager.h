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
    float maxVolume = 50.0f;
    bool hasLoadedTrack = false;

    void fadeVolume(float startVolume, float endVolume, float seconds) {
        if (seconds <= 0.0f) {
            music.setVolume(endVolume);
            return;
        }

        using clock = std::chrono::steady_clock;

        auto startTime = clock::now();

        while (true) {
            auto now = clock::now();
            float elapsed = std::chrono::duration<float>(now - startTime).count();

            float t = elapsed / seconds;

            if (t > 1.0f) {
                t = 1.0f;
            }

            float currentVolume = startVolume + (endVolume - startVolume) * t;
            music.setVolume(currentVolume);

            if (t >= 1.0f) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

public:
    bool playMusicImmediately(const std::string& filePath, bool loop = true) {
        music.stop();

        if (!music.openFromFile(filePath)) {
            std::cout << "Failed to load music file: " << filePath << std::endl;
            hasLoadedTrack = false;
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
        bool loop = true,
        float fadeOutSeconds = 1.5f,
        float fadeInSeconds = 1.5f
    ) {
        if (hasLoadedTrack && currentTrack == filePath) {
            return true;
        }

        if (hasLoadedTrack) {
            fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
            music.stop();
        }

        if (!music.openFromFile(filePath)) {
            std::cout << "Failed to load music file: " << filePath << std::endl;
            hasLoadedTrack = false;
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

    void stopMusicWithFade(float fadeOutSeconds = 1.5f) {
        if (!hasLoadedTrack) {
            return;
        }

        fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
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


