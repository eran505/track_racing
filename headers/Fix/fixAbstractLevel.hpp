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
    Point _window;
    unordered_map<u_int32_t,containerFix> block_Q_table;
    bool highest_level=false;
    u_int32_t block_max_size=1;
    u_int32_t current_entry=-1;
    //Grid G;

public:

    fixAbstractLevel()= default;
    const Point& get_tolerance()const{return _tolerance;}
    const Point& get_Point_abstract()const{return _mini_gird_size;}
    explicit fixAbstractLevel(const Point &mini_gird_size)
    :_mini_gird_size(mini_gird_size)
    ,block_Q_table()
    {}
    fixAbstractLevel(const Point &mini_grid_szie,const Point& window)
    :_mini_gird_size(mini_grid_szie),
    _window(window)
    {}

    Point get_abstract_point(const Point& original_position)const{
        return original_position/_mini_gird_size;
    }
    u_int32_t get_entry_mini_grid(const Point &p)const
    {
        return this->_mini_gird_size[0]*p[0]+p[1]+(this->_mini_gird_size[1]*
        this->_mini_gird_size[0])*p[2];
    }
    void state_transformition(const State *s,State *trans)
    {
        s->getAbstractionState_inplace(_mini_gird_size,trans);
    }
    void inset_containerFix(u_int32_t num_id){
        this->block_Q_table.try_emplace(num_id,containerFix());
    }
    void get_dict(const State *s)
    {

    }
    containerFix& get_ref_containerFix()
    {
        if(auto pos = this->block_Q_table.find(current_entry);pos==this->block_Q_table.end())
            assert(false);
        return this->block_Q_table.operator[](current_entry);
    }

private:
    u_int32_t state_to_entry(const State *s)
    {
        if(block_max_size==0) return 0;
        std::vector<Point> l;
        s->getAllPos(l,this->_mini_gird_size);
    }

};



#endif //TRACK_RACING_FIXABSTRACTLEVEL_HPP
