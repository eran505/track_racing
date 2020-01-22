//
// Created by ise on 5.1.2020.
//
#include <fstream>
#include "util_game.hpp"
#include <unistd.h>
#ifndef TRACK_RACING_UTILCLASS_HPP
#define TRACK_RACING_UTILCLASS_HPP

string getExePath()
{
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    return string( result, (count > 0) ? count : 0 );
}

#endif //TRACK_RACING_UTILCLASS_HPP
