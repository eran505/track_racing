//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_CONTAINERFIX_HPP
#define TRACK_RACING_CONTAINERFIX_HPP

#include "util_game.hpp"

typedef std::vector<double> arr;
typedef std::unique_ptr<unordered_map<u_int64_t,arr>> qTbale_dict;

class containerFix{


public:
    Point lower=Point(0);
    Point upper=Point(0);
    std::unique_ptr<Grid> G = std::make_unique<Grid>();
    qTbale_dict q = std::make_unique<unordered_map<u_int64_t ,arr>>();
    //containerFix(){cout<<"[containerFix] constructor"<<endl;}
};


#endif //TRACK_RACING_CONTAINERFIX_HPP
