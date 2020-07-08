//
// Created by ERANHER on 8.7.2020.
//

#ifndef TRACK_RACING_FIXABSTRACT_HPP
#define TRACK_RACING_FIXABSTRACT_HPP
#define ASSERTME
#include <cassert>
#include "util_game.hpp"
#include "AlgoRtdp.h"
#include "util/utilClass.hpp"
template<typename T=double>
class FixLevelAbst{
    typedef std::vector<T> arr;
    vector<unordered_map<u_int64_t,arr>> block_Q_table;
    u_int16_t min_grid_box_size;
    Grid G;

};


class fixAbstractor{

    Point _fix;
    u_int16_t _level_index=0;
    vector<FixLevelAbst<double>> _levels;

    fixAbstractor(const configGame &conf,Point&& fix):_fix(fix)
    {
        #ifdef ASSERTME
        assert(assert_fix(conf.sizeGrid)); // fix can be dived the grid
        #endif

    }

    void match_state(const State* s)
    {
        
    }

    #ifdef ASSERTME
    inline bool assert_fix(const Point& grid_origanal)
    {
        if(_fix.any_ngative() or _fix.sumPoint()==0) return false;
        if((grid_origanal%_fix).sumPoint()==0) return true;
        return false;
    }
    #endif
};

#endif //TRACK_RACING_FIXABSTRACT_HPP
