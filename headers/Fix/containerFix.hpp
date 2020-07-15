//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_CONTAINERFIX_HPP
#define TRACK_RACING_CONTAINERFIX_HPP

#include "util_game.hpp"

class containerFix{


public:
    std::unique_ptr<Grid> G = nullptr;
    containerFix(){cout<<"build containerFix"<<endl;}
    containerFix(const containerFix& other) {}
    containerFix(containerFix&& other) noexcept {}

};


#endif //TRACK_RACING_CONTAINERFIX_HPP
