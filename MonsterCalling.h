//
// Created by Taylor Anderson on 6/18/26.
//

#ifndef ARMAGESTA_MONSTERCALLING_H
#define ARMAGESTA_MONSTERCALLING_H
#include <iostream>
#include <string>
#include <utility>
#include "randomness.h"
#include <memory>
#include <vector>
#include "Encounter.h"
#include "PlayerDatabase.h"
#include "BackgroundMusicManager.h"

using namespace std;

inline void encounterSlime(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<Slime>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterSkeleton(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<Skeleton>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterThornImp(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<ThornImp>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterMireLeech(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<MireLeech>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterGlassSpider(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<GlassSpider>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterIronScarab(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<IronScarab>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterEchoBat(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<EchoBat>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterAshenHound(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<AshenHound>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterGraveMoss(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<GraveMoss>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterHollowSquire(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<HollowSquire>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterLanternBearer(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<LanternBearer>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterAncientEnt(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<AncientEnt>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterReforgedKnight(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<ReforgedKnight>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterCandleWraith(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<CandleWraith>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterCathedralGargoyle(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<CathedralGargoyle>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterSiltHydra(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<SiltHydra>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterMarionetteCoven(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<MarionetteCoven>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterStarlessOracle(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<StarlessOracle>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterDuneMaw(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<DuneMaw>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterCrystalMatriarch(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<CrystalMatriarch>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterTideLeviathan(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<TideLeviathan>(playerIP, musicManager);
    encounter->haveCombat();
}

inline void encounterCinderDragon(Player& playerIP, BackgroundMusicManager& musicManager) {
    musicManager.changeMusicWithFade("assets/music/Action 2.mp3");
    unique_ptr<Encounter> encounter = make_unique<CinderDragon>(playerIP, musicManager);
    encounter->haveCombat();
}

#endif //ARMAGESTA_MONSTERCALLING_H
