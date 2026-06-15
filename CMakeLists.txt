cmake_minimum_required(VERSION 4.2)
project(Armagesta)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Armagesta
        Encounter.h
        main.cpp
        randomness.h
        Event.h)
