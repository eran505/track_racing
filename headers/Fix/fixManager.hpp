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

#include "fixAbstractLevel.hpp"

#include "AbsAgnet.hpp"


class fixManager {

    vector<pair<Point,Point>> _fix_list;
    u_int16_t _level_index = 0;
    vector<fixAbstractLevel> _levels;
    agentAbs _defender;
    std::unique_ptr<State> _transform_state;

public:
    fixManager(const configGame &conf, vector<pair<Point,Point>> levels,std::shared_ptr<Agent> &D,const State *s)
    : _fix_list(std::move(levels)),
    _defender(D),
    _transform_state(std::make_unique<State>(*s))
    {
        #ifdef ASSERTME
        assert(assert_fix(conf.sizeGrid)); // fix can be dived the grid
        #endif
        make_levels(conf.sizeGrid);
        tranform_state_inital(s);
    }

    void make_levels(const Point &gridSize) {
        const Point& orignal_size = gridSize;
        int ctr=0;
        for (const auto &[window,cell_size]: _fix_list)
        {
            _levels.emplace_back(cell_size,window,orignal_size,ctr==0);
            ctr++;
        }
        _levels[_level_index].inset_containerFix(0); // init the map of the higher level
        cout<<"";
    }
    static bool equle_or_less(const Point &one, const Point &other) {
        for (auto i = 0; i < one.capacity; ++i) {
            if (one[i] <= other[i])
                return false;
        }
        return true;
    }

    void switch_scope(int num){
        _level_index += num;
        #ifdef ASSERTME
        assert(_level_index > 0 and _level_index < _levels.size());
        #endif
    }

    fixAbstractLevel& get_level() {
        #ifdef ASSERTME
        return _levels.at(_level_index);
        #else
        return _levels[_level_index];
        #endif
    }

#ifdef ASSERTME

    inline bool assert_fix(const Point &grid_origanal) {
        for (const auto &[item,cell]:_fix_list) {
            if (item.any_ngative() or item.sumPoint() == 0) return false;
            if ((grid_origanal % item).sumPoint() == 0) return true;
        }
        return false;
    }
#endif

    Point get_diff_abstract_A_D(const State *s) const {
        vector<Point> vecPoses;
        _transform_state->getAllPos(vecPoses);
        return (vecPoses[0] - vecPoses[1]).AbsPoint();
    }



    bool check_condition_for_down_scope(const Point &dif) const {
        const Point &tol = _levels[_level_index].get_tolerance();
        return equle_or_less(dif, tol);//if the tol<dif [->] not down_scope

    }

    bool check_condition_for_up_scope() const {
        return this->_defender.is_inbound(_transform_state.get());
    }

    void apply_action_to_state(const Point &action, State *s) const {
        s->applyAction(this->_defender.get_id(), action, this->_defender.get_max_speed());
    }
    void tranform_state_inital(const State *s)
    {
        Point abstract_point = _levels[_level_index].get_Point_abstract();
        //TODO: at the moment the abstract speed doesn't handle properly !!!!!
        _transform_state->transform_state_inplace(abstract_point);
        // change the grid


    }
    /** switch levels if need  **/
    void managing(const State *s) const {
        //TODO: need to inital the tranform state with the right grid for bound checking

        Point dif = get_diff_abstract_A_D(s);
        //is down scope
        auto is_down = check_condition_for_down_scope(dif);
        //is up scope
        auto is_up = check_condition_for_up_scope();

    }
    void down_scope(const State *s)
    {
        _defender.update_stack(); // do backup update
        _defender.retrun_dict(this->_levels[_level_index]); // return the dict
        _level_index++;
        #ifdef ASSERTME
        assert(_level_index<_levels.size());
        #endif
        _defender.set_dict(_levels[_level_index]);
    }
    void make_action(State *s) {
        // change the state
        this->_levels[_level_index].state_transformition(s, this->_transform_state.get());
        // get an action
        this->_defender.make_move(this->_transform_state.get());

        // update q table
        // backup update if need

        //get the last executed action
        const Point &last_action = _defender.get_last_action();
        // apply the action on the actual state
        apply_action_to_state(last_action, s);
    }
    void reset()
    {
        _level_index=0;



    }

};


#endif //TRACK_RACING_FIXMANAGER_HPP
