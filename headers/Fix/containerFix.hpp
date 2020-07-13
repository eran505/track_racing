//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_CONTAINERFIX_HPP
#define TRACK_RACING_CONTAINERFIX_HPP

#include "util_game.hpp"

template<typename Val=double,size_t fix_size=27>
class containerFix{
    typedef std::vector<Val> arr;
    std::unique_ptr<unordered_map<u_int64_t,arr>> Q_table;

    containerFix();

};

#endif //TRACK_RACING_CONTAINERFIX_HPP
