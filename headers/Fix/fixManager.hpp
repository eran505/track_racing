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
    Grid *G= nullptr;
public:
    fixManager(const configGame &conf, vector<pair<Point,Point>> &&levels,std::shared_ptr<Agent> &D,const State *s)
    : _fix_list(std::move(levels)),
    _defender(D),
    _transform_state(std::make_unique<State>(*s))
    {
        #ifdef ASSERTME
        assert(assert_fix(conf.sizeGrid)); // fix can be dived the grid
        #endif

        make_levels(conf.sizeGrid,s);
        tranform_state_inital(s);
    }
    fixManager()= default;
    void make_levels(const Point &gridSize,const State *s) {
        Point orignal_size = gridSize;
        auto gird_size = Point(4,4,1);
        int ctr=0;
        for (const auto &[window,cell_size]: _fix_list)
        {
            auto& levl =_levels.emplace_back(cell_size,window,orignal_size,gird_size,ctr==0);
            levl.set_gird_in_Grid_transition(s->g_grid);
            if(ctr>0)
                levl.window_generator(_fix_list[ctr-1].first);
            ctr++;
        }
        inset_the_first_grid_level();
        cout<<"";
    }
    static bool equle_or_less(const Point &one, const Point &other) {
        for (auto i = 0; i < one.capacity; ++i) {
            if (one[i] >= other[i])
                return false;
        }
        return true;
    }
    u_int32_t get_ctr_insetrtion()
    {
        u_int32_t  num=0;
        for(int i=0;i<_levels.size();++i)
        {
            cout<<"  lv["<<i<<"]="<<_levels[i].get_ctr_insert();
            num+=_levels[i].get_ctr_insert();
        }
        cout<<"  sum:"<<num<<endl;
        return num;
    }
    void inset_the_first_grid_level()
    {
        auto old_window = _levels[_level_index].get_window();
        _levels[_level_index].set_window(Point(1,1,1));
        _levels[_level_index].insert_containerFix(0); // init the map of the higher level
        _levels[_level_index].set_window(old_window);
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

    static Point get_diff_abstract_A_D(const std::vector<Point> &vecPoses) {
        return (vecPoses[0] - vecPoses[1]).AbsPoint();
    }

    bool check_condition_for_down_scope(std::vector<Point> &vecPoses) {
        bool is_down = _levels[_level_index].is_collison(vecPoses[0],vecPoses[1]);
        if(is_down)
            down_scope(vecPoses);
        return is_down;
    }

    [[nodiscard]] bool check_condition_for_up_scope(vector<Point> &l) {
        for(const Point &p:l)
        {
            if(G->is_wall(this->_levels[_level_index].transform_point(p)))
            {
                up_scope(l);
                return true;
            }
        }
        return false;
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
    bool managing(State *sOriginal) {
        //TODO: need to inital the tranform state with the right grid for bound checking

        std::vector<Point> vecPoses;
        sOriginal->getAllPos(vecPoses);

        auto is_up = check_condition_for_up_scope(vecPoses);
        if(is_up)
        {
            cout<<"[up]"<<"\n";
        }
        auto is_down = check_condition_for_down_scope(vecPoses);
        if(is_down)
        {
            cout<<"[down]"<<"\n";
        }
        sOriginal->g_grid=G;
        if(!is_down and !is_up)
            return false;
        return true;
    }
    void down_scope(const std::vector<Point> &vecPoses)
    {
        _defender.update_stack(); // do backup update
        _defender.retrun_dict(this->_levels[_level_index]); // return the dict
        _level_index++;
        #ifdef ASSERTME
        assert(_level_index<_levels.size());
        assert(vecPoses.size()==2);
        #endif
        G = _levels[_level_index].get_grid_and_change_entry(vecPoses[0],vecPoses[1],G->get_lower());
        _defender.set_dict(_levels[_level_index]);
        //_defender.set_offset(G->get_lower());
        this->change_agnet_abstraction();
    }
    void up_scope( std::vector<Point> &lPoses)
    {

        _defender.update_stack(); // do backup update
        _defender.retrun_dict(this->_levels[_level_index]); // return the dict
        _level_index--;
        #ifdef ASSERTME
        assert(_level_index>=0);
        assert(lPoses.size()==2);
        #endif
        G = _levels[_level_index].get_grid_and_change_entry(lPoses[0],lPoses[1],G->get_lower());
        _defender.set_dict(_levels[_level_index]);
        this->change_agnet_abstraction();
    }
    void make_action(State *s) {
        // change the state
        //this->_levels[_level_index].state_transformition(s, this->_transform_state.get());
        // get an action
        this->_defender.make_move(s);
        //TODO:maybe send abstraction and up_cast in bellman_eq function (elegant)

    }
    void reset(State *s)
    {
        _level_index=0;
        this->_defender.set_dict(_levels[_level_index]);
        this->change_agnet_abstraction();
        G=_levels[_level_index].get_grid();
    }
    void change_agnet_abstraction()
    {
        this->_defender.change_abstrct_point(
                _levels[_level_index].get_offset(),
                _levels[_level_index].get_Point_abstract(),
                _levels[_level_index].get_window()
                );
    }
    void end()
    {
        this->_defender.end(_levels[_level_index]);
    }


    void offset_and_abstract_state(vector<Point> &l)
    {
        for(Point& p: l)
        {
            p-=this->G->get_lower();
            #ifdef ASSERTME
            assert(!p.is_negative());
            #endif
            p/=_levels[_level_index].get_Point_abstract();

            cout<<p.to_str()<<", ";
        }
        cout<<endl;
    }
};


#endif //TRACK_RACING_FIXMANAGER_HPP
