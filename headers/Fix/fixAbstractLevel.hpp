//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_FIXABSTRACTLEVEL_HPP
#define TRACK_RACING_FIXABSTRACTLEVEL_HPP
#include "util_game.hpp"
#include "State.hpp"
#include "containerFix.hpp"

class fixAbstractLevel{
    Point _tolerance;
    Point _mini_gird_size;
    unordered_map<u_int32_t,containerFix<double,27>> block_Q_table;
    //Grid G;

public:
    const Point& get_tolerance()const{return _tolerance;}
    const Point& get_Point_abstract()const{return _mini_gird_size;}
    explicit fixAbstractLevel(const Point &abs )
    :_mini_gird_size(abs)
    ,block_Q_table()
    {}

    Point get_abstract_point(const Point& original_position)const{
        return original_position/_mini_gird_size;
    }
    u_int32_t get_entry_mini_grid(const Point &p)const
    {
        return this->_mini_gird_size[0]*p[0]+p[1]+(this->_mini_gird_size[1]*
        this->_mini_gird_size[0])*p[2];
    }

};



#endif //TRACK_RACING_FIXABSTRACTLEVEL_HPP
