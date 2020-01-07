//
// Created by ise on 5.1.2020.
//
#include <fstream>
#include "util_game.hpp"
#ifndef TRACK_RACING_UTILCLASS_HPP
#define TRACK_RACING_UTILCLASS_HPP


class utilObject{


public:
    void foo(){
        std::ofstream out("/home/ise/car_model/exp/DATA/file.txt");

        std::for_each(std::begin(M), std::end(M),
                      [&out](const std::pair<const std::string, int>& element) {
                          out << element.first << " " << element.second << std::endl;
                      }
        );
    }

};


#endif //TRACK_RACING_UTILCLASS_HPP
