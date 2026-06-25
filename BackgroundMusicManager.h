//
// Created by Taylor Anderson on 6/21/26.
//

#ifndef ARMAGESTA_BACKGROUNDMUSICMANAGER_H
#define ARMAGESTA_BACKGROUNDMUSICMANAGER_H

#include <SFML/Audio.hpp>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

class BackgroundMusicManager {
private:
    sf::Music music;

    std::string requestedTrack;
    std::string currentTrack;
    float maxVolume = 35.0f;
    bool hasLoadedTrack = false;

    [[nodiscard]] static bool fileExists(const std::string& filePath) {
        std::error_code ignoredError;
        return std::filesystem::exists(std::filesystem::path(filePath), ignoredError);
    }

    [[nodiscard]] static std::string pathWithExtension(const std::string& filePath, const std::string& extension) {
        std::filesystem::path path(filePath);
        path.replace_extension(extension);
        return path.string();
    }

    [[nodiscard]] static bool pathEndsWith(const std::string& text, const std::string& ending) {
        if (ending.size() > text.size()) {
            return false;
        }

        return std::equal(ending.rbegin(), ending.rend(), text.rbegin(), [](const char a, const char b) {
            return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
        });
    }

    [[nodiscard]] std::string chooseBestMusicFile(const std::string& filePath) const {
        // SFML can play MP3s, but MP3 files often contain encoder padding/clicks that become
        // very obvious during ambience loops. When an .ogg or .wav copy exists beside the .mp3,
        // Armagesta will automatically use that cleaner version instead.
        if (pathEndsWith(filePath, ".mp3")) {
            const std::vector<std::string> cleanerAlternatives = {
                pathWithExtension(filePath, ".ogg"),
                pathWithExtension(filePath, ".wav")
            };

            for (const std::string& alternative : cleanerAlternatives) {
                if (fileExists(alternative)) {
                    return alternative;
                }
            }
        }

        return filePath;
    }

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

            // Smoothstep keeps the volume from jumping sharply at the start/end of a fade.
            const float smoothedT = t * t * (3.0f - 2.0f * t);
            const float currentVolume = startVolume + (endVolume - startVolume) * smoothedT;
            music.setVolume(currentVolume);

            if (t >= 1.0f) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(12));
        }
    }

    bool loadAndStartTrack(const std::string& askedForPath, const bool loop, const float startingVolume) {
        const std::string resolvedPath = chooseBestMusicFile(askedForPath);

        if (!music.openFromFile(resolvedPath)) {
            std::cout << "Failed to load music file: " << resolvedPath << std::endl;
            hasLoadedTrack = false;
            requestedTrack.clear();
            currentTrack.clear();
            return false;
        }

        requestedTrack = askedForPath;
        currentTrack = resolvedPath;
        hasLoadedTrack = true;

        music.setLooping(loop);
        music.setVolume(0.0f);
        music.play();

        // A tiny silent pre-roll hides the click some decoders make right as a stream starts.
        std::this_thread::sleep_for(std::chrono::milliseconds(70));
        music.setVolume(startingVolume);

        return true;
    }

public:
    // Looping is back on by default. The manager still avoids restarting the same track,
    // which prevents extra start-clicks while wandering through the same ambience region.
    bool playMusicImmediately(const std::string& filePath, bool loop = true) {
        if (hasLoadedTrack && requestedTrack == filePath) {
            music.setLooping(loop);
            music.setVolume(maxVolume);
            return true;
        }

        music.setVolume(0.0f);
        music.stop();
        return loadAndStartTrack(filePath, loop, maxVolume);
    }

    bool changeMusicWithFade(
        const std::string& filePath,
        bool loop = true,
        float fadeOutSeconds = 0.45f,
        float fadeInSeconds = 0.45f
    ) {
        if (hasLoadedTrack && requestedTrack == filePath) {
            music.setLooping(loop);
            return true;
        }

        if (hasLoadedTrack) {
            fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
            music.stop();
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }

        if (!loadAndStartTrack(filePath, loop, 0.0f)) {
            return false;
        }

        fadeVolume(0.0f, maxVolume, fadeInSeconds);
        return true;
    }

    void stopMusicWithFade(float fadeOutSeconds = 0.45f) {
        if (!hasLoadedTrack) {
            return;
        }

        fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
        music.stop();
        hasLoadedTrack = false;
        requestedTrack.clear();
        currentTrack.clear();
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

    [[nodiscard]] std::string getRequestedTrack() const {
        return requestedTrack;
    }
};

#endif // ARMAGESTA_BACKGROUNDMUSICMANAGER_H
