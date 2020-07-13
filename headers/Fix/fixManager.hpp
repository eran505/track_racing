//
// Created by ERANHER on 8.7.2020.
//

#ifndef TRACK_RACING_FIXMANAGER_HPP
#define TRACK_RACING_FIXMANAGER_HPP
#define ASSERTME
#include <cassert>
#include "util_game.hpp"
#include "headers/Abstract/AlgoRtdp.h"
#include "util/utilClass.hpp"
#include "AbsAgnet.hpp"

#include "fixAbstractLevel.hpp"



/**
 * 16*16 --> 4*4
**/

class fixManager{

    std::vector<Point> _fix_list;
    u_int16_t _level_index=0;
    vector<fixAbstractLevel> _levels;
    agentAbs _defender;

    fixManager(const configGame &conf,vector<Point> levels):_fix_list(std::move(levels))
    {
        #ifdef ASSERTME
        assert(assert_fix(conf.sizeGrid)); // fix can be dived the grid
        #endif

    }
    void make_levels()
    {
        for(const auto& i : _fix_list )
            _levels.emplace_back(i);

    }
    static bool equle_or_less(const Point& one,const Point& other )
    {
        for(auto i=0;i<one.capacity;++i)
        {
            if(one[i]<other[i])
                return false;
        }
        return true;
    }

    void switch_scope(int num)
    {
        _level_index+=num;
        #ifdef ASSERTME
        assert(_level_index>0 and _level_index<_levels.size());
        #endif
    }

    fixAbstractLevel& get_level()
    {
        #ifdef ASSERTME
        return _levels.at(_level_index);
        #else
        return _levels[_level_index];
        #endif
    }

    #ifdef ASSERTME
    inline bool assert_fix(const Point& grid_origanal)
    {
        for(const auto& item:_fix_list)
        {
            if(item.any_ngative() or item.sumPoint()==0) return false;
            if((grid_origanal%item).sumPoint()==0) return true;
        }
        return false;
    }
    Point get_list_pos_abstract(const State *s) const
    {
        vector<Point> vecPoses;
        s->getAllPos(vecPoses, _levels[_level_index].get_Point_abstract());
        return (vecPoses[0] - vecPoses[1]).AbsPoint();
    }
    #endif
    bool check_condition_for_down_scope(const State *s) const
    {
        Point dif = get_list_pos_abstract(s); //diff between D and A (abstract)
        const Point &tol = _levels[_level_index].get_tolerance();
        return equle_or_less(dif,tol);//if the tol<dif [->] not down_scope

    }
    bool check_condition_for_up_scope(const State *s)const{
        auto Point_bouds = _levels[_level_index].bounds();
    }
public:
    /** switch levels if need  **/
    void managing(const State *s)
    {
        //is down scope
        auto is_down = check_condition_for_down_scope(s);
        //is up scope
        auto is_up = check_condition_for_up_scope(s);

    }

};


#endif //TRACK_RACING_FIXMANAGER_HPP
