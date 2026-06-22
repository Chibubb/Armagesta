cmake_minimum_required(VERSION 4.2)
project(Armagesta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)

FetchContent_MakeAvailable(SFML)

include_directories(.)

add_executable(Armagesta
        Encounter.h
        main.cpp
        randomness.h
        Events.h
        PlayerDatabase.h
        rulesForImplementation.h
        MonsterCalling.h
        BackgroundMusicManager.h
)

target_link_libraries(Armagesta PRIVATE
        SFML::Audio
)