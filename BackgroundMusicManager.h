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
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace ArmagestaMusic {
    inline std::string musicPath(const std::string& fileName) {
        return "assets/music/" + fileName;
    }

    inline std::string defaultAmbience() {
        return musicPath("Light Ambient 1 (Loop).mp3");
    }

    inline std::string biomeAmbience(const char biomeType) {
        switch (biomeType) {
            case 'F': return musicPath("Light Ambient 1 (Loop).mp3");      // Forest: gentle, readable, green exploration bed.
            case 'D': return musicPath("Ambient 8.mp3");                  // Desert: bright, exposed, wind-scoured.
            case 'C': return musicPath("Dark Ambient 4.mp3");             // Caves: low, hollow, stone-heavy.
            case 'R': return musicPath("Ambient 3.mp3");                  // Old Redwoods: deep, earthy, ancient.
            case 'W': return musicPath("Dark Ambient 2.mp3");             // Swamp: tense, wet, uneasy.
            case 'I': return musicPath("Night Ambient 5 (Loop).mp3");     // Citadel: haunted structure and ritual weight.
            case 'B': return musicPath("Light Ambience 2.mp3");           // Beach: open air and water.
            case 'M': return musicPath("Night Ambient 4 (Loop).mp3");     // Mountains: thin, cold, high atmosphere.

            case 'S': return musicPath("Ambient 10.mp3");                 // Sand Pit landmark.
            case 'G': return musicPath("Light Ambient 3 (Loop).mp3");     // Crystal Geode landmark.
            case 'O': return musicPath("Light Ambient 5 (Loop).mp3");     // Oldest Redwood landmark.
            case 'H': return musicPath("Dark Ambient 5.mp3");             // Witch's Hut landmark.
            case 'T': return musicPath("Night Ambient 2 (Loop).mp3");     // The Throne landmark.
            case 'Y': return musicPath("Light Ambient 4 (Loop).mp3");     // Coral Reef landmark.
            case 'L': return musicPath("Dark Ambient 1.mp3");             // Dragon's Lair landmark.
            default:  return defaultAmbience();
        }
    }

    inline std::string combatTrackForEnemy(const std::string& enemyName) {
        if (enemyName == "Slime") return musicPath("Action 1 Loop.mp3");
        if (enemyName == "Skeleton") return musicPath("Action 1 Loop.mp3");
        if (enemyName == "Thorn Imp") return musicPath("Action 3 Loop.mp3");
        if (enemyName == "Mire Leech") return musicPath("Action 2 Loop.mp3");
        if (enemyName == "Glass Spider") return musicPath("Action 3 Loop.mp3");
        if (enemyName == "Iron Scarab") return musicPath("Action 4 Loop.mp3");
        if (enemyName == "Echo Bat") return musicPath("Action 3 Loop.mp3");
        if (enemyName == "Ashen Hound") return musicPath("Action 5 Loop.mp3");
        if (enemyName == "Grave Moss") return musicPath("Action 2 Loop.mp3");
        if (enemyName == "Hollow Squire") return musicPath("Action 4 Loop.mp3");
        if (enemyName == "Lantern Bearer") return musicPath("Action 4 Loop.mp3");
        if (enemyName == "Ancient Ent") return musicPath("Action 2 Loop.mp3");
        if (enemyName == "Reforged Knight") return musicPath("Action 4 Loop.mp3");
        if (enemyName == "Candle Wraith") return musicPath("Action 5 Loop.mp3");
        if (enemyName == "Cathedral Gargoyle") return musicPath("Action 4 Loop.mp3");
        if (enemyName == "Silt Hydra") return musicPath("Action 5 Loop.mp3");
        if (enemyName == "Marionette Coven") return musicPath("Action 5 Loop.mp3");
        if (enemyName == "Starless Oracle") return musicPath("Action 5 Loop.mp3");
        if (enemyName == "Dune Maw") return musicPath("Action 2 Loop.mp3");
        if (enemyName == "Crystal Matriarch") return musicPath("Action 3 Loop.mp3");
        if (enemyName == "Tide Leviathan") return musicPath("Action 4 Loop.mp3");
        if (enemyName == "Cinder Dragon") return musicPath("Action 5 Loop.mp3");

        return musicPath("Action 3 Loop.mp3");
    }

    inline std::string victoryCue() {
        return musicPath("Victory.mp3");
    }

    inline std::string deathCue() {
        return musicPath("Death.mp3");
    }

    inline std::string completionCue() {
        return musicPath("Complete.mp3");
    }

    inline std::string strangeCue() {
        return musicPath("Strange.mp3");
    }

    inline std::string landmarkCue() {
        return musicPath("Fx 1.mp3");
    }

    inline std::string bossSoulCue() {
        return musicPath("Fx 2.mp3");
    }

    inline std::string dangerCue() {
        return musicPath("Fx 3.mp3");
    }
}

class BackgroundMusicManager {
private:
    // sf::Music streams from disk. On some systems that can create a tiny periodic
    // buffer click, which is what you were hearing as a metronome. Armagesta now
    // loads only the currently-needed track into memory and plays it with sf::Sound.
    sf::SoundBuffer musicBuffer;
    sf::SoundBuffer cueBuffer;
    std::optional<sf::Sound> musicSound;
    std::optional<sf::Sound> cueSound;

    std::string requestedTrack;
    std::string currentTrack;
    std::string requestedCue;
    std::string currentCue;
    float maxVolume = 35.0f;
    float cueVolumeMultiplier = 1.0f;
    bool hasLoadedTrack = false;
    bool hasLoadedCue = false;

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

    [[nodiscard]] static std::string stripAssetsPrefix(const std::string& filePath) {
        const std::string prefix = "assets/music/";
        if (filePath.rfind(prefix, 0) == 0) {
            return filePath.substr(prefix.size());
        }
        return std::filesystem::path(filePath).filename().string();
    }

    [[nodiscard]] static std::vector<std::string> possibleMusicLocations(const std::string& filePath) {
        const std::string fileName = stripAssetsPrefix(filePath);
        std::vector<std::string> candidates;

        // Normal CLion/CMake run location after assets are copied into cmake-build-debug.
        candidates.push_back("assets/music/" + fileName);

        // Useful when the game is run from cmake-build-debug but assets have not been copied yet.
        candidates.push_back("../assets/music/" + fileName);

        // Useful for alternate working directories.
        candidates.push_back("../../assets/music/" + fileName);
        candidates.push_back("music/" + fileName);
        candidates.push_back(fileName);

        // If the incoming path was already something custom, try it exactly too.
        if (filePath != candidates.front()) {
            candidates.push_back(filePath);
        }

        return candidates;
    }

    [[nodiscard]] std::string findExistingMusicFile(const std::string& askedForPath) const {
        for (const std::string& candidate : possibleMusicLocations(askedForPath)) {
            const std::string preferredCandidate = chooseBestMusicFile(candidate);
            if (fileExists(preferredCandidate)) {
                return preferredCandidate;
            }
            if (fileExists(candidate)) {
                return candidate;
            }
        }

        return chooseBestMusicFile(askedForPath);
    }

    static void printMissingMusicHelp(const std::string& askedForPath, const std::string& resolvedPath) {
        std::cout << "Failed to load music file: " << resolvedPath << std::endl;
        std::cout << "Expected this track in one of these places:" << std::endl;
        for (const std::string& candidate : possibleMusicLocations(askedForPath)) {
            std::cout << "  - " << candidate << std::endl;
        }
        std::cout << "Tip: put the music files in your source project's assets/music folder, then Reload CMake or rebuild."
                  << std::endl;
    }

    [[nodiscard]] std::string chooseBestMusicFile(const std::string& filePath) const {
        // If an .ogg or .wav copy exists beside an .mp3, prefer it. This is still useful
        // for clean loop files, but playback is no longer streamed with sf::Music.
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

    void setMainVolume(const float newVolume) {
        if (musicSound.has_value()) {
            musicSound->setVolume(std::clamp(newVolume, 0.0f, 100.0f));
        }
    }

    void setCueVolume(const float newVolume) {
        if (cueSound.has_value()) {
            cueSound->setVolume(std::clamp(newVolume, 0.0f, 100.0f));
        }
    }

    void fadeVolume(float startVolume, float endVolume, float seconds) {
        if (!musicSound.has_value()) {
            return;
        }

        if (seconds <= 0.0f) {
            setMainVolume(endVolume);
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
            setMainVolume(currentVolume);

            if (t >= 1.0f) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(12));
        }
    }

    bool loadAndStartTrack(const std::string& askedForPath, const bool loop, const float startingVolume) {
        const std::string resolvedPath = findExistingMusicFile(askedForPath);

        if (musicSound.has_value()) {
            musicSound->stop();
            musicSound.reset();
        }

        if (!musicBuffer.loadFromFile(resolvedPath)) {
            printMissingMusicHelp(askedForPath, resolvedPath);
            hasLoadedTrack = false;
            requestedTrack.clear();
            currentTrack.clear();
            return false;
        }

        requestedTrack = askedForPath;
        currentTrack = resolvedPath;
        hasLoadedTrack = true;

        musicSound.emplace(musicBuffer);
        musicSound->setLooping(loop);
        musicSound->setVolume(std::clamp(startingVolume, 0.0f, 100.0f));
        musicSound->play();

        return true;
    }

public:
    // Looping is on by default. The manager still avoids restarting the same track,
    // which prevents extra start-clicks while wandering through the same ambience region.
    bool playMusicImmediately(const std::string& filePath, bool loop = true) {
        if (hasLoadedTrack && requestedTrack == filePath && musicSound.has_value()) {
            musicSound->setLooping(loop);
            setMainVolume(maxVolume);
            return true;
        }

        if (musicSound.has_value()) {
            musicSound->stop();
            musicSound.reset();
        }
        return loadAndStartTrack(filePath, loop, maxVolume);
    }

    bool changeMusicWithFade(
        const std::string& filePath,
        bool loop = true,
        float fadeOutSeconds = 0.45f,
        float fadeInSeconds = 0.45f
    ) {
        if (hasLoadedTrack && requestedTrack == filePath && musicSound.has_value()) {
            musicSound->setLooping(loop);
            return true;
        }

        if (hasLoadedTrack && musicSound.has_value()) {
            fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
            musicSound->stop();
            musicSound.reset();
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }

        if (!loadAndStartTrack(filePath, loop, 0.0f)) {
            return false;
        }

        fadeVolume(0.0f, maxVolume, fadeInSeconds);
        return true;
    }

    bool changeAmbienceForBiome(const char biomeType, const bool loop = true) {
        return changeMusicWithFade(ArmagestaMusic::biomeAmbience(biomeType), loop, 0.65f, 0.80f);
    }

    bool changeCombatMusicForEnemy(const std::string& enemyName) {
        return changeMusicWithFade(ArmagestaMusic::combatTrackForEnemy(enemyName), true, 0.25f, 0.35f);
    }

    bool playCueImmediately(const std::string& filePath, const float volumeMultiplier = 1.0f, const bool restartIfSameCue = true) {
        if (!restartIfSameCue && hasLoadedCue && requestedCue == filePath && cueSound.has_value()) {
            return true;
        }

        const std::string resolvedPath = findExistingMusicFile(filePath);

        if (cueSound.has_value()) {
            cueSound->stop();
            cueSound.reset();
        }

        if (!cueBuffer.loadFromFile(resolvedPath)) {
            printMissingMusicHelp(filePath, resolvedPath);
            hasLoadedCue = false;
            requestedCue.clear();
            currentCue.clear();
            return false;
        }

        requestedCue = filePath;
        currentCue = resolvedPath;
        cueVolumeMultiplier = volumeMultiplier;
        hasLoadedCue = true;

        cueSound.emplace(cueBuffer);
        cueSound->setLooping(false);
        cueSound->setVolume(std::clamp(maxVolume * cueVolumeMultiplier, 0.0f, 100.0f));
        cueSound->play();
        return true;
    }

    void stopMusicWithFade(float fadeOutSeconds = 0.45f) {
        if (!hasLoadedTrack || !musicSound.has_value()) {
            return;
        }

        fadeVolume(maxVolume, 0.0f, fadeOutSeconds);
        musicSound->stop();
        musicSound.reset();
        hasLoadedTrack = false;
        requestedTrack.clear();
        currentTrack.clear();
    }

    void stopCue() {
        if (cueSound.has_value()) {
            cueSound->stop();
            cueSound.reset();
        }
        hasLoadedCue = false;
        requestedCue.clear();
        currentCue.clear();
    }

    void pauseMusic() {
        if (musicSound.has_value()) {
            musicSound->pause();
        }
        if (cueSound.has_value()) {
            cueSound->pause();
        }
    }

    void resumeMusic() {
        if (hasLoadedTrack && musicSound.has_value()) {
            musicSound->play();
        }
        if (hasLoadedCue && cueSound.has_value()) {
            cueSound->play();
        }
    }

    void setVolume(float newVolume) {
        maxVolume = std::clamp(newVolume, 0.0f, 100.0f);
        setMainVolume(maxVolume);
        setCueVolume(maxVolume * cueVolumeMultiplier);
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

    [[nodiscard]] std::string getCurrentCue() const {
        return currentCue;
    }

    [[nodiscard]] std::string getRequestedCue() const {
        return requestedCue;
    }
};
#endif // ARMAGESTA_BACKGROUNDMUSICMANAGER_H
