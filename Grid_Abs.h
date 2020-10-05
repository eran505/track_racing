//
// Created by eranhe on 16/07/2020.
//

#ifndef TRACK_RACING_GRID_ABS_H
#define TRACK_RACING_GRID_ABS_H

#include "util_game.hpp"
#include "Grid.hpp"
class Grid_Abs: Grid{
    //upper
    //lower
    Point _window_collision;
public:
    explicit Grid_Abs(const Point& window_collision);
};

Grid_Abs::Grid_Abs(const Point &window_collision) {
    _window_collision=window_collision;
}


#endif //TRACK_RACING_GRID_ABS_H
