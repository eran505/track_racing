cmake_minimum_required(VERSION 3.15)
project(Racing_car)

set(CMAKE_CXX_STANDARD 14)

add_executable(Racing_car main.cpp headers/Grid.hpp Grid.cpp headers/util_game.hpp util_game.cpp headers/MdpPlaner.hpp MdpPlaner.cpp headers/Agent.hpp Agent.cpp headers/State.hpp State.cpp headers/Policy/Policy.hpp headers/Policy/Dog.hpp headers/Game.hpp Game.cpp headers/Game.hpp headers/transition_container.hpp)